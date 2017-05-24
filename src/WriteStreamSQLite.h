/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file WriteStreamSQLite.h
 *
 ***********************************************************************/

#pragma once

#include "SymbolMask.h"
#include "SymbolTable.h"
#include "WriteStream.h"

#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

#include <sqlite3.h>

namespace souffle {

class WriteStreamSQLite : public WriteStream {
public:
    WriteStreamSQLite(const std::string& dbFilename, const std::string& relationName,
            const SymbolMask& symbolMask, const SymbolTable& symbolTable)
            : dbFilename(dbFilename), relationName(relationName), symbolMask(symbolMask),
              symbolTable(symbolTable) {
        openDB();
        createTables();
        prepareStatements();
        //        executeSQL("BEGIN TRANSACTION", db);
    }

    void writeNextTuple(const RamDomain* tuple) override {
        if (symbolMask.getArity() == 0) {
            return;
        }

        for (size_t i = 0; i < symbolMask.getArity(); i++) {
            int32_t value;
            if (symbolMask.isSymbol(i)) {
                value = getSymbolTableID(tuple[i]);
            } else {
                value = (int32_t)tuple[i];
            }
            if (sqlite3_bind_int(insertStatement, i + 1, value) != SQLITE_OK) {
                throwError("SQLite error in sqlite3_bind_text: ");
            }
        }
        if (sqlite3_step(insertStatement) != SQLITE_DONE) {
            throwError("SQLite error in sqlite3_step: ");
        }
        sqlite3_clear_bindings(insertStatement);
        sqlite3_reset(insertStatement);
    }

    ~WriteStreamSQLite() override {
        sqlite3_finalize(insertStatement);
        sqlite3_finalize(symbolInsertStatement);
        sqlite3_finalize(symbolSelectStatement);
        sqlite3_close(db);
    }

private:
    void executeSQL(const std::string& sql, sqlite3* db) {
        assert(db && "Database connection is closed");

        char* errorMessage = nullptr;
        /* Execute SQL statement */
        int rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errorMessage);
        if (rc != SQLITE_OK) {
            std::stringstream error;
            error << "SQLite error in sqlite3_exec: " << sqlite3_errmsg(db) << "\n";
            error << "SQL error: " << errorMessage << "\n";
            error << "SQL: " << sql << "\n";
            sqlite3_free(errorMessage);
            throw std::invalid_argument(error.str());
        }
    }

    void throwError(std::string message) {
        std::stringstream error;
        error << message << sqlite3_errmsg(db) << "\n";
        throw std::invalid_argument(error.str());
    }

    uint64_t getSymbolTableIDFromDB(int index) {
        if (sqlite3_bind_text(symbolSelectStatement, 1, symbolTable.resolve(index), -1, SQLITE_TRANSIENT) !=
                SQLITE_OK) {
            throwError("SQLite error in sqlite3_bind_text: ");
        }
        if (sqlite3_step(symbolSelectStatement) != SQLITE_ROW) {
            throwError("SQLite error in sqlite3_step: ");
        }
        uint64_t rowid = sqlite3_column_int64(symbolSelectStatement, 0);
        sqlite3_clear_bindings(symbolSelectStatement);
        sqlite3_reset(symbolSelectStatement);
        return rowid;
    }
    uint64_t getSymbolTableID(int index) {
        if (dbSymbolTable.count(index) != 0) {
            return dbSymbolTable[index];
        }

        if (sqlite3_bind_text(symbolInsertStatement, 1, symbolTable.resolve(index), -1, SQLITE_TRANSIENT) !=
                SQLITE_OK) {
            throwError("SQLite error in sqlite3_bind_text: ");
        }
        // Either the insert succeeds and we have a new row id or it already exists and a select is needed.
        uint64_t rowid;
        if (sqlite3_step(symbolInsertStatement) != SQLITE_DONE) {
            // The symbol already exists so select it.
            rowid = getSymbolTableIDFromDB(index);
        } else {
            rowid = sqlite3_last_insert_rowid(db);
        }
        sqlite3_clear_bindings(symbolInsertStatement);
        sqlite3_reset(symbolInsertStatement);

        dbSymbolTable[index] = rowid;
        return rowid;
    }

    void openDB() {
        if (sqlite3_open(dbFilename.c_str(), &db) != SQLITE_OK) {
            throwError("SQLite error in sqlite3_open");
        }
        sqlite3_extended_result_codes(db, 1);
        executeSQL("PRAGMA synchronous = OFF", db);
        executeSQL("PRAGMA journal_mode = MEMORY", db);
    }

    void prepareStatements() {
        prepareInsertStatement();
        prepareSymbolInsertStatement();
        prepareSymbolSelectStatement();
    }
    void prepareSymbolInsertStatement() {
        std::stringstream insertSQL;
        insertSQL << "INSERT INTO " << symbolTableName;
        insertSQL << " VALUES(null,@V0);";
        const char* tail = nullptr;
        if (sqlite3_prepare_v2(db, insertSQL.str().c_str(), -1, &symbolInsertStatement, &tail) != SQLITE_OK) {
            throwError("SQLite error in sqlite3_prepare_v2: ");
        }
    }

    void prepareSymbolSelectStatement() {
        std::stringstream selectSQL;
        selectSQL << "SELECT id FROM " << symbolTableName;
        selectSQL << " WHERE symbol = @V0;";
        const char* tail = nullptr;
        if (sqlite3_prepare_v2(db, selectSQL.str().c_str(), -1, &symbolSelectStatement, &tail) != SQLITE_OK) {
            throwError("SQLite error in sqlite3_prepare_v2: ");
        }
    }

    void prepareInsertStatement() {
        std::stringstream insertSQL;
        insertSQL << "INSERT INTO _" << relationName << " VALUES ";
        insertSQL << "(@V0";
        for (unsigned int i = 1; i < symbolMask.getArity(); i++) {
            insertSQL << ",@V" << i;
        }
        insertSQL << ");";
        const char* tail = nullptr;
        if (sqlite3_prepare_v2(db, insertSQL.str().c_str(), -1, &insertStatement, &tail) != SQLITE_OK) {
            throwError("SQLite error in sqlite3_prepare_v2: ");
        }
    }

    void createTables() {
        createRelationTable();
        createRelationView();
        createSymbolTable();
    }

    void createRelationTable() {
        std::stringstream createTableText;
        createTableText << "CREATE TABLE IF NOT EXISTS '_" << relationName << "' (";
        if (symbolMask.getArity() > 0) {
            createTableText << "'0' INTEGER";
            for (unsigned int i = 1; i < symbolMask.getArity(); i++) {
                createTableText << ",'" << std::to_string(i) << "' ";
                createTableText << "INTEGER";
            }
        }
        createTableText << ");";
        executeSQL(createTableText.str(), db);
        executeSQL("DELETE FROM '_" + relationName + "';", db);
    }

    void createRelationView() {
        // Create view with symbol strings resolved
        std::stringstream createViewText;
        createViewText << "CREATE VIEW IF NOT EXISTS '" << relationName << "' AS ";
        std::stringstream projectionClause;
        std::stringstream fromClause;
        fromClause << "'_" << relationName << "'";
        std::stringstream whereClause;
        bool firstWhere = true;
        for (unsigned int i = 0; i < symbolMask.getArity(); i++) {
            std::string columnName = std::to_string(i);
            if (i != 0) {
                projectionClause << ",";
            }
            if (!symbolMask.isSymbol(i)) {
                projectionClause << "'_" << relationName << "'.'" << columnName << "'";
            } else {
                projectionClause << "'_symtab_" << columnName << "'.symbol AS '" << columnName << "'";
                fromClause << ",'" << symbolTableName << "' AS '_symtab_" << columnName << "'";
                if (!firstWhere) {
                    whereClause << " AND ";
                } else {
                    firstWhere = false;
                }
                whereClause << "'_" << relationName << "'.'" << columnName << "' = "
                            << "'_symtab_" << columnName << "'.id";
            }
        }
        createViewText << "SELECT " << projectionClause.str() << " FROM " << fromClause.str();
        if (!firstWhere) {
            createViewText << " WHERE " << whereClause.str();
        }
        createViewText << ";";
        executeSQL(createViewText.str(), db);
    }
    void createSymbolTable() {
        std::stringstream createTableText;
        createTableText << "CREATE TABLE IF NOT EXISTS '" << symbolTableName << "' ";
        createTableText << "(id INTEGER PRIMARY KEY, symbol TEXT UNIQUE);";
        executeSQL(createTableText.str(), db);
    }

    const std::string& dbFilename;
    const std::string& relationName;
    const std::string symbolTableName = "__SymbolTable";
    const SymbolMask& symbolMask;
    const SymbolTable& symbolTable;

    std::unordered_map<uint64_t, uint64_t> dbSymbolTable;
    sqlite3_stmt* insertStatement;
    sqlite3_stmt* symbolInsertStatement;
    sqlite3_stmt* symbolSelectStatement;
    sqlite3* db;
};

class WriteSQLiteFactory : public WriteStreamFactory {
public:
    std::unique_ptr<WriteStream> getWriter(const SymbolMask& symbolMask, const SymbolTable& symbolTable,
            const IODirectives& ioDirectives) override {
        std::string dbName = ioDirectives.get("dbname");
        std::string relationName = ioDirectives.getRelationName();
        return std::unique_ptr<WriteStreamSQLite>(
                new WriteStreamSQLite(dbName, relationName, symbolMask, symbolTable));
    }
    const std::string& getName() const override {
        static const std::string name = "sqlite";
        return name;
    }
    ~WriteSQLiteFactory() override = default;
};

} /* namespace souffle */
