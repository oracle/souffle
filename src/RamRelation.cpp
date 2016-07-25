/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file RamRelation.cpp
 *
 * Implements classes for indexed tables. Tuples are stored in blocks 
 * chained with a simply linked list. Indexes follow the subscriber model, 
 * i.e., an index is notified if a new tuple is inserted into the table. 
 * Iterators are provided.
 *
 ***********************************************************************/

#include <stdio.h>
#include <stdarg.h>
#include <memory.h>
#include <string.h>

#include <iostream>
#include <list>

#include "RamIndex.h"
#include "RamRelation.h"
#include "SymbolTable.h"
#include "StringPool.h"

namespace souffle {


/* print table in csv format */ 
void RamRelation::store(std::ostream &os, const SymbolTable& symTable, const SymbolMask& mask) const {
    size_t cols = getArity(); 
    if (cols == 0 && !empty()) {
        os << "()\n";
        return;
    }
    for(iterator it=begin(); it!=end(); ++it) {
        const RamDomain *tuple = (*it);
        if (mask.isSymbol(0)) {
            std::string s = symTable.resolve(tuple[0]);
            os << s ;
        } else {
            os << tuple[0];
        }
        for(size_t col=1;col < cols;col++) {
            if (mask.isSymbol(col)) {
                std::string s = symTable.resolve(tuple[col]);
                os << "\t" << s;
            } else {
                os << "\t" << (int32_t) tuple[col];
            }
        }
        os << "\n";
    }
}

/* input table from csv file */ 
bool RamRelation::load(std::istream &is, SymbolTable& symTable, const SymbolMask& mask) {
    bool error = false; 
    auto arity = getArity();
    while (!is.eof()) {
        std::string line;
        RamDomain tuple[arity];

        getline(is,line);
        if (is.eof()) break;

        size_t start = 0, end = 0;
        for(uint32_t col=0;col<arity;col++) { 
            end = line.find('\t', start);
            if ((size_t)end == std::string::npos) {
                end = line.length();
            }
            std::string element;
            if (start <=  end && (size_t)end <= line.length() ) {
                element = line.substr(start,end-start);
                if (element == "") {
                    element = "n/a";
                }
            } else {
                error = true; 
                element = "n/a";
            }
            if (mask.isSymbol(col)) {
                tuple[col] = symTable.lookup(element.c_str());
            } else {
                tuple[col] = atoi(element.c_str());
            }
            start = end+1;
        }
        if (end != line.length()) {
            error = true; 
        } 
        if (!exists(tuple)) { 
            insert(tuple);
        }
    }
    return error;
}

} // end of namespace souffle

