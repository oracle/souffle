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

    struct HashFunction {
            std::size_t operator() (const std::size_t &hash) const { return hash; }
    };

    struct HashEqual {
            bool operator() (const std::size_t & h1, const std::size_t & h2) const { return h1 == h2; }
    };

    std::unordered_map<size_t, const char*, HashFunction, HashEqual> symbolTable;

    static inline const size_t symbolTableHash(const char* str) {
        std::hash<std::string> hashFunction;
        return hashFunction(str);
    }

    /** String pointer comparison class for SymbolTable */
    struct StringCmp {
        bool operator()(const char* lhs, const char* rhs) const  {
            return strcmp(lhs, rhs) < 0; 
        }
    };

    /** Map integer to string */ 
    std::vector<char *> numToStr;

    /** Map strings kept in the pool to numbers */
    std::map<const char *, size_t, StringCmp> strToNum;

    inline void copyAll() {
        // for (auto & symbol : symbolTable) symbol.second = strdup(symbol.second);
        for(auto& cur : numToStr) cur = strdup(cur);
        for(auto& cur : strToNum) const_cast<char*&>(cur.first) = numToStr[cur.second];
    }

    inline void freeAll() {
        // for(auto symbol : symbolTable) free((void*) symbol.second);
        for(auto cur : numToStr) free(cur);
    }

public:

    SymbolTable() { }

    SymbolTable(const SymbolTable& other)
    //  : symbolTable(other.symbolTable) {
        : numToStr(other.numToStr), strToNum(other.strToNum) {
        copyAll();
    }

    SymbolTable(SymbolTable&& other) {
        // symbolTable.swap(other.symbolTable);
        numToStr.swap(other.numToStr);
        strToNum.swap(other.strToNum);
    }

    /** Destructor cleaning up strings */
    virtual ~SymbolTable() {
        freeAll();
    }

    /** Add support for an assignment operator */
    SymbolTable& operator=(const SymbolTable& other) {
        if (this == &other) return *this;
        freeAll();
        // symbolTable = other.symbolTable;
        numToStr = other.numToStr;
        strToNum = other.strToNum;
        copyAll();
        return *this;
    }

    /** Add support for assignments from r-value references */
    SymbolTable& operator=(SymbolTable&& other) {
        // symbolTable.swap(other.symbolTable);
        numToStr.swap(other.numToStr);
        strToNum.swap(other.strToNum);
        return *this;
    }

    /** Look-up a string given by a pointer to @p std::string in the pool and convert it to an index */
    size_t lookup(const char *symbol) {
        // const size_t hash = symbolTableHash(symbol);
        // if (symbolTable.find(hash) == symbolTable.end()) {
            // const char* str = strdup(symbol);
            // symbolTable[hash] = str;
        // }
        // return hash;
        auto lease = access.acquire();
        (void) lease; // avoid warning;
        size_t result;
        auto it = strToNum.find(symbol);
        if (it != strToNum.end()) {
            result = (*it).second;
        } else {
            result = numToStr.size();
            char *str = strdup(symbol);  // generate a new string
            strToNum[str] = result;
            numToStr.push_back(str);
        }
        return result;
    }

    /** Lookup an index and convert it to a string */
    const char* resolve(size_t i) const {
        auto lease = access.acquire();
        (void) lease; // avoid warning;
        // return symbolTable.find(hash)->second;
        return numToStr[i];
    }

    /* return size */ 
    size_t size() const {
        // return symbolTable.size();
        return numToStr.size();
    }

    /** insert symbols from a constant string table */ 
    void insert(const char **symbols, size_t n) {
        auto lease = access.acquire();
        (void) lease; // avoid warning;
        // symbolTable.reserve(symbolTable.size() + n);
        for(size_t idx = 0; idx < n; idx++) {
            const char* symbol = symbols[idx];
            // const size_t hash = symbolTableHash(symbol);
            // if (symbolTable.find(hash) == symbolTable.end()) {
            if (strToNum.find(symbol) == strToNum.end()) {
                char *str = strdup(symbol);  // generate a new string
                // symbolTable[hash] = str;
                strToNum[str] = numToStr.size();
                numToStr.push_back(str);
            }
        }
    }

    /** inserts a single symbol into this table */
    void insert(const char* symbol) {
        auto lease = access.acquire();
        (void) lease; // avoid warning;
        // const size_t hash = symbolTableHash(symbol);
        // if (symbolTable.find(hash) == symbolTable.end()) {
        if (strToNum.find(symbol) == strToNum.end()) {
            char *str = strdup(symbol);  // generate a new string
            // symbolTable[hash] = str;
            strToNum[str] = numToStr.size();
            numToStr.push_back(str);
        }
    }

    void print(std::ostream& out) const {
        out << "SymbolTable: {\n\t";
        out << join(symbolTable, "\n\t", [](std::ostream& out, const std::pair<std::size_t, const char*>& entry) {
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


