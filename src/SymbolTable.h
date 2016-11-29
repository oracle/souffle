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

    /** Map integer to string */
    std::vector<char *> numToStr;

    /** Map strings kept in the pool to numbers */
    std::unordered_map<std::string, size_t> strToNum;

    inline void copyAll() {
        for(auto& cur : numToStr) cur = strdup(cur);
        for(auto& cur : strToNum) const_cast<std::string&>(cur.first) = numToStr[cur.second];
    }

    inline void freeAll() {
        for(auto cur : numToStr) free(cur);
    }

    inline const size_t newSymbol(const char* symbol) {
        size_t idx;
        auto it = strToNum.find(symbol);
        if (it == strToNum.end()) {
            char *str = strdup(symbol);
            idx = numToStr.size();
            strToNum[str] = idx;
            numToStr.push_back(str);
        } else {
            idx = it->second;
        }
        return idx;
    }

public:

    SymbolTable() { }

    SymbolTable(const SymbolTable& other)
        : numToStr(other.numToStr)
        , strToNum(other.strToNum) {
        copyAll();
    }

    SymbolTable(SymbolTable&& other) {
        numToStr.swap(other.numToStr);
        strToNum.swap(other.strToNum);
    }

    virtual ~SymbolTable() {
        freeAll();
    }

    /** Add support for an assignment operator */
    SymbolTable& operator=(const SymbolTable& other) {
        if (this == &other) return *this;
        freeAll();
        numToStr = other.numToStr;
        strToNum = other.strToNum;
        copyAll();
        return *this;
    }

    /** Add support for assignments from r-value references */
    SymbolTable& operator=(SymbolTable&& other) {
        numToStr.swap(other.numToStr);
        strToNum.swap(other.strToNum);
        return *this;
    }

    /** Look-up a string given by a pointer to @p std::string in the pool and convert it to an index */
    const size_t lookup(const char *symbol) {
        auto lease = access.acquire();
        (void) lease; // avoid warning;
        return newSymbol(symbol);
    }

    /** Lookup an index and convert it to a string */
    const char* resolve(const size_t idx) const {
        auto lease = access.acquire();
        (void) lease; // avoid warning;
        if (idx >= size()) {
            // TODO: use different error reporting here!!
            std::cerr << "Error: Symbol table index out of bounds.";
            exit(1);
        }
        return numToStr[idx];
    }

    /* return size */ 
    size_t size() const {
        return numToStr.size();
    }

    /** insert symbols from a constant string table */ 
    void insert(const char **symbols, const size_t n) {
        auto lease = access.acquire();
        (void) lease; // avoid warning;
        strToNum.reserve(size() + n);
        numToStr.reserve(size() + n);
        for (size_t idx = 0; idx < n; idx++)
            newSymbol(symbols[idx]);
    }

    /** inserts a single symbol into this table */
    void insert(const char* symbol) {
        auto lease = access.acquire();
        (void) lease; // avoid warning;
        newSymbol(symbol);
    }

    void print(std::ostream& out) const {
        out << "SymbolTable: {\n\t";
        out << join(strToNum, "\n\t", [](std::ostream& out, const std::pair<std::string, std::size_t>& entry) {
            out << entry.first << "\t => " << entry.second;
        }) << "\n";
        out << "}\n";
    }

    friend std::ostream& operator<<(std::ostream& out, const SymbolTable& table) {
        table.print(out);
        return out;
    }

};

}
