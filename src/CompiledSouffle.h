/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All Rights reserved
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
 * @file CompiledSouffle.h
 *
 * Main include file for generated C++ classes of Souffle
 *
 ***********************************************************************/

#pragma once

#include <iostream>
#include <regex>
#include <map>
#include <array>
#include "CompiledRamRelation.h"
#include "CompiledRamRecord.h"
#include "CompiledRamOptions.h"
#include "RamLogger.h"
#include "ParallelUtils.h"
#include "SouffleInterface.h"
#include "SymbolTable.h"
#include "SqliteRelationWriter.h"
#if defined(_OPENMP) 
#include <omp.h>
#endif

namespace souffle {

/**
 * Relation wrapper used internally in the generated Datalog program 
 */
template <uint32_t id, class RelType, class TupleType, size_t Arity, bool IsInputRel, bool IsOutputRel> 
class RelationWrapper : public Relation { 
private:
    RelType &relation;
    SymbolTable &symTable;
    std::string name;
    std::array<const char *,Arity> tupleType; 
    std::array<const char *,Arity> tupleName;

    class iterator_wrapper : public iterator_base {
        typename RelType::iterator it;
        Relation *relation; 
        tuple t;
    public:
        iterator_wrapper(uint32_t arg_id, Relation* rel, const typename RelType::iterator &arg_it) : iterator_base(arg_id), it(arg_it), relation(rel), t(rel) {
        }
        void  operator++() { 
            ++ it ; 
        } 
        tuple&  operator*()  { 
            t.rewind(); 
            for(size_t i=0;i<Arity;i++) {
                t[i]=(*it)[i];
            }
            return t;
        } 
        iterator_base* clone() const  {
            return new iterator_wrapper(*this);
        }
    protected:
        bool equal(const iterator_base& o) const {
            const iterator_wrapper & casted = static_cast<const iterator_wrapper&>(o);
            return it == casted.it;
        }
    };

public:
    RelationWrapper(RelType &r, SymbolTable &s, std::string name, const std::array<const char *, Arity> &t, const std::array<const char *, Arity> &n): relation(r), symTable(s), name(name), tupleType(t), tupleName(n) { }
    iterator begin() { return iterator(new iterator_wrapper(id,this,relation.begin())); }
    iterator end() { return iterator(new iterator_wrapper(id,this,relation.end())); }
    void insert(const tuple &arg) {
        TupleType t;
        assert(arg.size() == Arity && "wrong tuple arity");
        for(size_t i=0;i<Arity;i++) {
            t[i] = arg[i];
        }
        relation.insert(t);
    }
    bool contains(const tuple &arg) const {
        TupleType t;
        assert(arg.size() == Arity && "wrong tuple arity");
        for(size_t i=0;i<Arity;i++) {
            t[i] = arg[i];
        }
        return relation.contains(t);
    }
    bool isInput() const { return IsInputRel; }
    bool isOutput() const { return IsOutputRel; }
    std::size_t size() { return relation.size(); }
    std::string getName() const {
        return name;
    }
    const char* getAttrType(size_t arg) const {
        assert(0<=arg && arg < Arity && "attribute out of bound"); 
        return tupleType[arg];
    }
    const char* getAttrName(size_t arg) const {
        assert(0<=arg && arg < Arity && "attribute out of bound");
        return tupleName[arg];
    }
    size_t getArity() const { return Arity; }
    SymbolTable &getSymbolTable() const { return symTable; } 
};

}
