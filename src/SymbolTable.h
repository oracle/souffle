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

    /** A lock to synchronize parallel accesses */
    mutable Lock access;

public:

    /** Private constructor */
    SymbolTable() { }

    SymbolTable(const SymbolTable& other)
        : numToStr(other.numToStr), strToNum(other.strToNum) {
        // clone all contained strings
        for(auto& cur : numToStr) cur = strdup(cur);
        for(auto& cur : strToNum) const_cast<char*&>(cur.first) = numToStr[cur.second];
    }

    SymbolTable(SymbolTable&& other) {
        numToStr.swap(other.numToStr);
        strToNum.swap(other.strToNum);
    }

    /** Destructor cleaning up strings */
    ~SymbolTable() {
        for(auto cur : numToStr) free(cur);
    }

    /** Add support for an assignment operator */
    SymbolTable& operator=(const SymbolTable& other) {
        // shortcut
        if (this == &other) return *this;

        // delete this content
        for(auto cur : numToStr) free(cur);

        // copy in other content
        numToStr = other.numToStr;
        strToNum = other.strToNum;
        for(auto& cur : numToStr) cur = strdup(cur);
        for(auto& cur : strToNum) const_cast<char*&>(cur.first) = numToStr[cur.second];

        // done
        return *this;
    }

    /** Add support for assignments from r-value references */
    SymbolTable& operator=(SymbolTable&& other) {
        // steal content of other
        numToStr.swap(other.numToStr);
        strToNum.swap(other.strToNum);
        return *this;
    }

    /** Look-up a string given by a pointer to @p std::string in the pool and convert it to an index */
    size_t lookup(const char *p) {
        size_t result;
        {
            auto lease = access.acquire();
            (void) lease; // avoid warning;

            auto it = strToNum.find(p);
            if (it != strToNum.end()) {
                result = (*it).second;
            } else {
                result = numToStr.size();
                char *str = strdup(p);  // generate a new string
                strToNum[str] = result;
                numToStr.push_back(str);
            }
        }
        return result;
    }

    /** Lookup an index and convert it to a string */
    const char* resolve(size_t i) const {
        auto lease = access.acquire();
        (void) lease; // avoid warning;
        return numToStr[i];
    }

    /* return size */ 
    size_t size() const {
        return numToStr.size();
    }

    /** insert symbols from a constant string table */ 
    void insert(const char **symbols, size_t n) {
        auto lease = access.acquire();
        (void) lease; // avoid warning;
        for(size_t idx=0; idx < n; idx++) {
            const char *p = symbols[idx];
            char *str = strdup(p);
            strToNum[str] = numToStr.size();
            numToStr.push_back(str);
        }
    }

    /** inserts a single symbol into this table */
    void insert(const char* symbol) {
        insert(&symbol, 1);
    }

    void print(std::ostream& out) const {
        out << "SymbolTable: {\n\t";
        out << join(strToNum, "\n\t", [](std::ostream& out, const std::pair<const char*,std::size_t>& entry) {
            out << entry.first << "\t => " << entry.second;
        }) << "\n";
        out << "}\n";
    }

    friend std::ostream& operator<<(std::ostream& out, const SymbolTable& table) {
        table.print(out);
        return out;
    }

    template<class Function>
    Function map(Function fn) const {
        for (size_t i = 0; i < numToStr.size(); ++i)
            fn(i, numToStr[i]);
        return std::move(fn);
    }
};
}
