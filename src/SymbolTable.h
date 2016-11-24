/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file SymbolTable.h
 *
 * Data container to store symbols of the Datalog program. 
 *
 ***********************************************************************/

#pragma once

#include <unordered_map>
#include <functional>
#include <map>
#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <string.h>

#include "Util.h"
#include "ParallelUtils.h"

namespace souffle {

/**
 * @class SymbolTable
 *
 * Global pool of re-usable strings
 *
 * SymbolTable stores Datalog symbols and converts them to numbers and vice versa.
 */
class SymbolTable {

    /** A lock to synchronize parallel accesses */
    mutable Lock access;

private:

    class HashFunction {
        public:
            std::size_t operator() (const std::size_t &hash) const { return hash; }
    };

    class HashEqual {
        public:
            bool operator() (const std::size_t & h1, const std::size_t & h2) const { return h1 == h2; }
    };

    std::unordered_map<size_t, const char*, HashFunction, HashEqual> symbolTable;

    inline size_t newSymbolTableEntry(const char* str) {
        size_t hash = getHashForString(str);
        if (symbolTable.find(hash) == symbolTable.end()) {
            char* newstr = strdup(str);
            symbolTable[hash] = newstr;
        }
        return hash;
    }

    inline size_t getHashForString(const char* ch) const {
        std::hash<std::string> hashFunction;
        return hashFunction(ch);
    }

    inline const char* getStringForHash(size_t hash) const {
        return symbolTable.find(hash)->second;
    }

    /** Copy the referenced strings into the table. */
    inline void copyAll() {
        for (auto & symbol : symbolTable) symbol.second = strdup(symbol.second);
    }

    /** Free all the strings referenced in the table. */
    inline void freeAll() {
        for(auto symbol : symbolTable) free((void*) symbol.second);
    }

public:

    SymbolTable() { }

    SymbolTable(const SymbolTable& other)
        : symbolTable(other.symbolTable) {
        copyAll();
    }

    SymbolTable(SymbolTable&& other) {
        symbolTable.swap(other.symbolTable);
    }

    /** Destructor cleaning up strings */
    virtual ~SymbolTable() {
        freeAll();
    }

    /** Add support for an assignment operator */
    SymbolTable& operator=(const SymbolTable& other) {
        if (this == &other) return *this;
        freeAll();
        symbolTable = other.symbolTable;
        copyAll();
        return *this;
    }

    /** Add support for assignments from r-value references */
    SymbolTable& operator=(SymbolTable&& other) {
        symbolTable.swap(other.symbolTable);
        return *this;
    }

    /** Look-up a string given by a pointer to @p std::string in the pool and convert it to an index */
    size_t lookup(const char *p) {
        size_t result;
        {
            auto lease = access.acquire();
            (void) lease; // avoid warning;
            result = newSymbolTableEntry(p);
        }
        return result;
    }

    /** Lookup an index and convert it to a string */
    const char* resolve(size_t hash) const {
        auto lease = access.acquire();
        (void) lease; // avoid warning;
        return getStringForHash(hash);
    }

    /* return size */
    size_t size() const {
        return symbolTable.size();
    }

    /** insert symbols from a constant string table */ 
    void insert(const char **symbols, size_t n) {
        auto lease = access.acquire();
        (void) lease; // avoid warning;
        for(size_t idx=0; idx < n; idx++) newSymbolTableEntry(symbols[idx]);
    }

    /** inserts a single symbol into this table */
    void insert(const char* symbol) {
        insert(&symbol, 1);
    }

    void print(std::ostream& out) const {
        out << "SymbolTable: {\n\t";
        out << join(symbolTable, "\n\t", [](std::ostream& out, const std::pair<std::size_t,const char*>& entry) {
            out << entry.second << "\t => " << entry.first;
        }) << "\n";
        out << "}\n";
    }

    friend std::ostream& operator<<(std::ostream& out, const SymbolTable& table) {
        table.print(out);
        return out;
    }

};
}
