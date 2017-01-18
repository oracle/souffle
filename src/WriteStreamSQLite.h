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

#include "WriteStream.h"

#include "SymbolMask.h"
#include "SymbolTable.h"

#include <sqlite3.h>

#include <memory>
#include <sstream>
#include <string>

namespace souffle {

class WriteStreamSQLite : public WriteStream {
public:
    WriteStreamSQLite(const std::string& dbFilename, const std::string& relationName,
            const SymbolMask& symbolMask, const SymbolTable& symbolTable)
            : dbFilename(dbFilename),
              relationName(relationName),
              symbolMask(symbolMask),
              symbolTable(symbolTable) {
        openDB();
        createRelationTable();
        prepareInsertStatement();
        //        executeSQL("BEGIN TRANSACTION", db);
    }

    virtual void writeNextTuple(const RamDomain* tuple) {
        if (symbolMask.getArity() == 0) {
            return;
        }

        for (size_t i = 0; i < symbolMask.getArity(); i++) {
            std::string symbol;
            if (symbolMask.isSymbol(i)) {
                symbol = symbolTable.resolve(tuple[i]);
            } else {
                symbol = std::to_string((int32_t)tuple[i]);
            }
            if (sqlite3_bind_text(insertStmt, i + 1, symbol.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK) {
                std::stringstream error;
                error << "SQLite error in sqlite3_bind_text: " << sqlite3_errmsg(db) << "\n";
                throw std::invalid_argument(error.str());
            }
        }
        if (sqlite3_step(insertStmt) != SQLITE_DONE) {
            std::stringstream error;
            error << "SQLite error in sqlite3_step: " << sqlite3_errmsg(db) << "\n";
            throw std::invalid_argument(error.str());
        }
        sqlite3_clear_bindings(insertStmt);
        sqlite3_reset(insertStmt);
    }

    virtual ~WriteStreamSQLite() {
        sqlite3_finalize(insertStmt);
        sqlite3_close(db);
    }

private:
    virtual void executeSQL(const std::string& sql, sqlite3* db) {
        assert(db && "Database connection is closed");

        char* errorMessage = 0;
        /* Execute SQL statement */
        int rc = sqlite3_exec(db, sql.c_str(), NULL, 0, &errorMessage);
        if (rc != SQLITE_OK) {
            std::stringstream error;
            error << "SQLite error in sqlite3_exec: " << sqlite3_errmsg(db) << "\n";
            error << "SQL error: " << errorMessage << "\n";
            error << "SQL: " << sql << "\n";
            sqlite3_free(errorMessage);
            throw std::invalid_argument(error.str());
        }
    }
    void openDB() {
        if (sqlite3_open(dbFilename.c_str(), &db) != SQLITE_OK) {
            std::stringstream error;
            error << "SQLite error in sqlite3_open: " << sqlite3_errmsg(db);
            throw std::invalid_argument(error.str());
        }
        sqlite3_extended_result_codes(db, 1);
        executeSQL("PRAGMA synchronous = OFF", db);
        executeSQL("PRAGMA journal_mode = MEMORY", db);
    }

    virtual void prepareInsertStatement() {
        std::stringstream insertSQL;
        insertSQL << "INSERT INTO _" << relationName << " VALUES ";
        insertSQL << "(@V0";
        for (unsigned int i = 1; i < symbolMask.getArity(); i++) {
            insertSQL << ",@V" << i;
        }
        insertSQL << ");";
        const char* tail = 0;
        if (sqlite3_prepare_v2(db, insertSQL.str().c_str(), -1, &insertStmt, &tail) != SQLITE_OK) {
            std::stringstream error;
            error << "SQLite error in sqlite3_prepare_v2: " << sqlite3_errmsg(db) << "\n";
            throw std::invalid_argument(error.str());
        }
    }
    virtual void createRelationTable() {
        std::stringstream createStmt;
        createStmt << "CREATE TABLE IF NOT EXISTS '_" << relationName << "' (";
        if (symbolMask.getArity() > 0) {
            createStmt << "'0' TEXT";
            for (unsigned int i = 1; i < symbolMask.getArity(); i++) {
                createStmt << ",'" << std::to_string(i) << "' ";
                createStmt << "TEXT";
            }
        }
        createStmt << ");";
        executeSQL(createStmt.str(), db);
        executeSQL("DELETE FROM '_" + relationName + "';", db);
    }

private:
    const std::string& dbFilename;
    const std::string& relationName;
    const SymbolMask& symbolMask;
    const SymbolTable& symbolTable;

    sqlite3_stmt* insertStmt;
    sqlite3* db;
};

} /* namespace souffle */
