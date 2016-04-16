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
 * @file RamOperation.cpp
 *
 * Implements the operation of a relational algebra query consisting of 
 * Search/Scan and a Project operation. The Search/Scan operation traverses
 * a table and/or check for condition of each tuple and/or uses an index. 
 *
 ***********************************************************************/

#include <stdio.h>
#include <memory.h>
#include <string>

#include <iostream>
#include <list>

#include "RamIndex.h"
#include "RamCondition.h"
#include "RamOperation.h"
#include "RamRecords.h"
#include "RamRelation.h"

namespace souffle {

/** add condition */
void RamOperation::addCondition(std::unique_ptr<RamCondition> c, RamOperation *root) {
    assert(c->getLevel() == level);

    if (condition) {
        condition = std::unique_ptr<RamCondition>(new RamAnd(std::move(condition), std::move(c)));
    } else {
        condition.swap(c);
    }
}

/** add condition */
void RamSearch::addCondition(std::unique_ptr<RamCondition> c, RamOperation *root) {
    assert(c->getLevel() >= level);

    if (c->getLevel() > level) {
        getNestedOperation()->addCondition(std::move(c),root);
        return;
    }

    // use base-class implementation
    RamOperation::addCondition(std::move(c), root);
}

namespace {

    /** get indexable element */
    std::unique_ptr<RamValue> getIndexElement(RamCondition *c, size_t &element, size_t level)
    {
        if(RamBinaryRelation *binRelOp = dynamic_cast<RamBinaryRelation *>(c)) {
            if (binRelOp->getOperator() == BinaryRelOp::EQ) {
                if (RamElementAccess *lhs=dynamic_cast<RamElementAccess *>(binRelOp->getLHS())){
                    RamValue *rhs = binRelOp->getRHS();
                    if (lhs->getLevel() == level && (rhs->isConstant() || rhs->getLevel() < level)) {
                        element = lhs->getElement();
                        return binRelOp->takeRHS();
                    }
                }
                if (RamElementAccess *rhs=dynamic_cast<RamElementAccess *>(binRelOp->getRHS())) {
                    RamValue *lhs = binRelOp->getLHS();
                    if (rhs->getLevel() == level && (lhs->isConstant() || lhs->getLevel() < level)) {
                        element = rhs->getElement();
                        return binRelOp->takeLHS();
                    }
                }
            }
        }
        return std::unique_ptr<RamValue>(nullptr);
    }


}

/** add condition */
void RamScan::addCondition(std::unique_ptr<RamCondition> c, RamOperation *root)
{
    // use condition to narrow scan if possible
    if(c->getLevel() == level) {
        size_t element;
        if (std::unique_ptr<RamValue> value = getIndexElement(c.get(), element, level)) {
            keys |= (1<<element);
            if (queryPattern[element] == NULL) {
                queryPattern[element] = std::move(value);
            } else {
                std::unique_ptr<RamValue> field(new RamElementAccess(level, element));
                RamSearch::addCondition(std::unique_ptr<RamCondition>(new RamBinaryRelation(BinaryRelOp::EQ, std::move(field), std::move(value))), root);
            }
            return;
        }
    }

    // otherwise: use default handling
    RamSearch::addCondition(std::move(c),root);
}


void RamScan::print(std::ostream &os, int tabpos) const {
    os << times('\t', tabpos);

    if (isPureExistenceCheck()) {

        os << "IF ∃ t" << level << " ∈ " << relation.getName() << " ";
        if (keys != 0) {
            os << "WITH ";
            bool first = true;
            for(size_t i=0; i<relation.getArity(); i++) {
                if(queryPattern[i] != NULL) {
                    if (first) first=false; else os << "and ";
                    os << "t" << level << "." << relation.getArg(i) << "=";
                    queryPattern[i]->print(os);
                    os << " ";
                }
            }
        }

    } else {

        if (keys == 0) {
            os << "SCAN " << relation.getName() << " AS t" << level << " ";
        }
        else{
            // Keys indicates index search?
            os << "SEARCH " << relation.getName() << " AS t" << level;
            os << " ON INDEX ";
            bool first = true;
            for(size_t i=0; i<relation.getArity(); i++) {
                if(queryPattern[i] != NULL) {
                    if (first) first=false; else os << "and ";
                    os << "t" << level << "." << relation.getArg(i) << "=";
                    queryPattern[i]->print(os);
                    os << " ";
                }
            } 
        }

    }
    if (auto condition = getCondition()) {
        os << "WHERE ";
        condition->print(os);
    }

    os << "\n";
    if (getNestedOperation() != NULL) {
        getNestedOperation()->print(os, tabpos+1);
    }
}


/*
 * Class Lookup
 */

/** print search */
void RamLookup::print(std::ostream &os, int tabpos) const {

    os << times('\t', tabpos);

    os << "UNPACK env(t" << refLevel << ", i" << refPos << ") INTO t" << getLevel();

    if(auto condition = getCondition()) {
        os << " WHERE ";
        condition->print(os);
    }

    os << " FOR \n";
    getNestedOperation()->print(os, tabpos+1);
}

/** add condition */
void RamAggregate::addCondition(std::unique_ptr<RamCondition> c, RamOperation *root)
{
    // use condition to narrow scan if possible
    if(c->getLevel() == level) {
        size_t element;
        if (std::unique_ptr<RamValue> value = getIndexElement(c.get(), element, level)) {
            if (element > 0 || relation.getName().find("__agg") == std::string::npos) { 
                keys |= (1<<element);
                if (pattern[element] == NULL) {
                    pattern[element] = std::move(value);
                } else {
                    std::unique_ptr<RamValue> field(new RamElementAccess(level, element));
                    RamSearch::addCondition(std::unique_ptr<RamCondition>(new RamBinaryRelation(BinaryRelOp::EQ, std::move(field), std::move(value))), root);
                }
            } else { 
                std::unique_ptr<RamValue> field(new RamElementAccess(level, element));
                std::unique_ptr<RamCondition> eq(new RamBinaryRelation(BinaryRelOp::EQ, std::move(field), std::move(value)));
                if (condition != NULL) { 
                    condition = std::unique_ptr<RamCondition>(new RamAnd(std::move(condition), std::move(eq)));
                } else { 
                    condition.swap(eq);
                } 
            } 
            return;
        }
    }

    // otherwise: use default handling
    RamSearch::addCondition(std::move(c),root);
}

/** print search */
void RamAggregate::print(std::ostream &os, int tabpos) const {

    os << times('\t', tabpos);

    switch(fun) {
    case MIN   : os << "MIN "; break;
    case MAX   : os << "MAX "; break;
    case COUNT : os << "COUNT "; break;
    }

    if (fun != COUNT) {
        os << *value << " ";
    }

    os << "AS t" << getLevel() << ".0 IN t" << getLevel() << " ∈ " << relation.getName();
    os << "(" << join(pattern,",",[&](std::ostream& out, const std::unique_ptr<RamValue> &value) {
        if (!value) out << "_"; else out << *value;
    }) << ")";

    if(auto condition = getCondition()) {
        os << " WHERE ";
        condition->print(os);
    }

    os << " FOR \n";
    getNestedOperation()->print(os, tabpos+1);
}

/*
 * Class Project
 */ 

/* print projection */ 
void RamProject::print(std::ostream &os, int tabpos) const {
    const std::string tabs(tabpos, '\t');

    // support table-less options
    if (auto condition = getCondition()) {
        os << "IF ";
        condition->print(os);
        os << " THEN ";
    }

    os << tabs << "PROJECT ("
            << join(values, ", ", print_deref<std::unique_ptr<RamValue>>())
       << ") INTO " << relation.getName();

    if (hasFilter()) {
        os << " UNLESS IN " << getFilter().getName();
    }
}

} // end of namespace souffle

