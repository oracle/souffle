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
 * @file RamCondition.h
 *
 * Defines a class for evaluating conditions in the Relational Algebra
 * Machine. 
 *
 ***********************************************************************/

#pragma once

#include <stdlib.h>

#include <string>
#include <sstream>
#include <algorithm>

#include "RamRelation.h"
#include "RamNode.h"
#include "RamIndex.h"
#include "RamValue.h"
#include "SymbolTable.h"
#include "BinaryOperator.h"

/** abstract class condition */ 
class RamCondition : public RamNode {
public:

    RamCondition(RamNodeType type) : RamNode(type) {}

    virtual ~RamCondition() {}

    /** get level of condition */ 
    virtual size_t getLevel() = 0;  

};

/** class for matching a string with a pattern */ 
class RamAnd : public RamCondition {
    /** left-hand side */ 
    std::unique_ptr<RamCondition> lhs;

    /** right-hand side */ 
    std::unique_ptr<RamCondition> rhs;
public:      
    RamAnd(std::unique_ptr<RamCondition> l, std::unique_ptr<RamCondition> r)
        : RamCondition(RN_And), lhs(std::move(l)), rhs(std::move(r)) {}

    ~RamAnd() { }

    const RamCondition& getLHS() const {
        return *lhs;
    }

    const RamCondition& getRHS() const {
        return *rhs;
    }

    virtual void print(std::ostream &os) const {
        lhs->print(os); 
        os << " and "; 
        rhs->print(os); 
    }
    virtual size_t getLevel() {
        return std::max(lhs->getLevel(),rhs->getLevel());  
    }

    /** Obtains a list of child nodes */
    virtual std::vector<const RamNode*> getChildNodes() const {
        return { lhs.get(), rhs.get() };
    }
};

/** abstract class for binary operation/relations on values */ 
class RamBinaryRelation : public RamCondition {
private:
    BinaryRelOp op;
    /** left-hand side */ 
    std::unique_ptr<RamValue> lhs;
    /** right-hand side */ 
    std::unique_ptr<RamValue> rhs;

public:
    RamBinaryRelation(BinaryRelOp op, std::unique_ptr<RamValue> l, std::unique_ptr<RamValue> r)
        : RamCondition(RN_BinaryRelation), op(op), lhs(std::move(l)), rhs(std::move(r)) {}

    virtual ~RamBinaryRelation() { }

    virtual void print(std::ostream &os) const {
        lhs->print(os); 
        os << " " << getSymbolForBinaryRelOp(op) << " ";
        rhs->print(os); 
    }
    virtual size_t getLevel() {
        return std::max(lhs->getLevel(),rhs->getLevel());  
    }
    /** get left-hand side */ 
    RamValue *getLHS() const {
        return lhs.get();
    } 
    /** get right-hand side */ 
    RamValue *getRHS() const {
        return rhs.get();
    }

    std::unique_ptr<RamValue> takeLHS() {
        return std::move(lhs);
    }

    std::unique_ptr<RamValue> takeRHS() {
        return std::move(rhs);
    }

    /** set LHS */ 
    void setLHS(std::unique_ptr<RamValue> l) {
        lhs.swap(l);
    }
    /** set RHS */ 
    void setRHS(std::unique_ptr<RamValue> r) {
        rhs.swap(r);
    }

    BinaryRelOp getOperator() const {
        return op;
    }

    /** Obtains a list of child nodes */
    virtual std::vector<const RamNode*> getChildNodes() const {
        return { lhs.get(), rhs.get() };
    }

};

/** check whether a tuple (pattern) does not exist in a relation */
class RamNotExists : public RamCondition {

    /** the relation to be accessed */
    RamRelationIdentifier relation;

    /** the restricted fields -- null if undefined */
    std::vector<std::unique_ptr<RamValue>> values;

    /** A reference to the utilized index */
    mutable RamIndex* index;

public:
    RamNotExists(const RamRelationIdentifier& rel)
        : RamCondition(RN_NotExists), relation(rel), index(nullptr) {}

    ~RamNotExists() { }

    const RamRelationIdentifier& getRelation() const {
        return relation;
    }

    std::vector<RamValue*> getValues() const {
        return toPtrVector(values);
    }

    void addArg(std::unique_ptr<RamValue> v) {
        values.push_back(std::move(v));
    }

    virtual size_t getLevel() {
        size_t level = 0; 
        for(const auto& cur : values) {
            if (cur) level = std::max(level, cur->getLevel());
        }
        return level; 
    }

    /** Obtains the index utilized by this operation */
    RamIndex* getIndex() const {
        return index;
    }

    /** updates the index utilized by this operation */
    void setIndex(RamIndex* index) const {
        this->index = index;
    }

    void print(std::ostream &os) const {
        os << "(" << join(values, ",", [](std::ostream& out, const std::unique_ptr<RamValue> &value) {
            if (!value) out << "_";
            else out << *value;
        }) << ") ∉ " << relation.getName();
    }

    /** Obtains a list of child nodes */
    virtual std::vector<const RamNode*> getChildNodes() const {
        std::vector<const RamNode*> res;
        for(const auto& cur : values) res.push_back(cur.get());
        return res;
    }

    SearchColumns getKey() const {
        SearchColumns res = 0;
        for(unsigned i=0; i<values.size(); i++) {
            if (values[i]) res |= (1 << i);
        }
        return res;
    }

    bool isTotal() const {
        for(const auto& cur : values) {
            if (!cur) return false;
        }
        return true;
    }
};

/** check whether a given relation is empty or not*/
class RamEmpty : public RamCondition {

    /** the relation to be accessed */
    RamRelationIdentifier relation;

public:
    RamEmpty(const RamRelationIdentifier& rel)
        : RamCondition(RN_Empty), relation(rel) {}

    ~RamEmpty() {}

    const RamRelationIdentifier& getRelation() const {
        return relation;
    }

    virtual size_t getLevel() {
        return 0;       // can be in the top level
    }

    void print(std::ostream &os) const {
        os << relation.getName() << " ≠ ∅";
    }

    /** Obtains a list of child nodes */
    virtual std::vector<const RamNode*> getChildNodes() const {
        return std::vector<const RamNode*>();
    }
};
