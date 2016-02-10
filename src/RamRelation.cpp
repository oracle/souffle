/*
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All Rights reserved
 * 
 * The Universal Permissive License (UPL), Version 1.0
 * 
 * Subject to the condition set forth below, permission is hereby granted to any person obtaining a copy of this software,
 * associated documentation and/or data (collectively the "Software"), free of charge and under any and all copyright rights in the 
 * Software, and any and all patent rights owned or freely licensable by each licensor hereunder covering either (i) the unmodified 
 * Software as contributed to or provided by such licensor, or (ii) the Larger Works (as defined below), to deal in both
 * 
 * (a) the Software, and
 * (b) any piece of software and/or hardware listed in the lrgrwrks.txt file if one is included with the Software (each a “Larger
 * Work” to which the Software is contributed by such licensors),
 * 
 * without restriction, including without limitation the rights to copy, create derivative works of, display, perform, and 
 * distribute the Software and make, use, sell, offer for sale, import, export, have made, and have sold the Software and the 
 * Larger Work(s), and to sublicense the foregoing rights on either these or other terms.
 * 
 * This license is subject to the following condition:
 * The above copyright notice and either this complete permission notice or at a minimum a reference to the UPL must be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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




/* print table in csv format */ 
void RamRelation::store(std::ostream &os, const SymbolTable& symTable, const SymbolMask& mask) const {
    size_t cols = getArity(); 
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
