/*
 * Copyright (c) 2013, Oracle and/or its affiliates. All Rights reserved
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
 * @file AstClause.cpp
 *
 * Implements methods of class Clause that represents rules including facts,
 * predicates, and queries in a Datalog program.
 *
 ***********************************************************************/

#include <string.h>
#include <stdlib.h>

#include <sstream>
#include <set>
#include <string>

#include "AstClause.h"
#include "AstRelation.h"
#include "AstProgram.h"
#include "AstLiteral.h"
#include "AstVisitor.h"

namespace souffle {

/*
 * Clause
 */

/* Add literal to body */
void AstClause::addToBody(std::unique_ptr<AstLiteral> l) {
    if (dynamic_cast<AstAtom*>(l.get())) {
        atoms.push_back(std::unique_ptr<AstAtom>(static_cast<AstAtom*>(l.release())));
    } else if (dynamic_cast<AstNegation*>(l.get())) {
        negations.push_back(std::unique_ptr<AstNegation>(static_cast<AstNegation*>(l.release())));
    } else if (dynamic_cast<AstConstraint*>(l.get())) {
        constraints.push_back(std::unique_ptr<AstConstraint>(static_cast<AstConstraint*>(l.release())));
    } else {
        assert(false && "Unsupported literal type!");
    }
}

/* Set the head of clause to h */
void AstClause::setHead(std::unique_ptr<AstAtom> h) {
    ASSERT(!head && "Head is already set");
    head = std::move(h);
}

AstLiteral* AstClause::getBodyLiteral(size_t idx) const {
    if (idx < atoms.size()) return atoms[idx].get();
    idx -= atoms.size();
    if (idx < negations.size()) return negations[idx].get();
    idx -= negations.size();
    return constraints[idx].get();
}

std::vector<AstLiteral*> AstClause::getBodyLiterals() const {
    std::vector<AstLiteral*> res;
    for(auto &cur : atoms) res.push_back(cur.get());
    for(auto &cur : negations) res.push_back(cur.get());
    for(auto &cur : constraints) res.push_back(cur.get());
    return res;
}

bool AstClause::isFact() const {
    // there must be a head
    if (head == NULL) return false;
    // there must not be any body clauses
    if (getBodySize() != 0) return false;
    // and there are no aggregates
    bool hasAggregates = false;
    visitDepthFirst(*head, [&](const AstAggregator& cur) {
        hasAggregates = true;
    });
    return !hasAggregates;
}

void AstClause::print(std::ostream &os) const {
    if (head != NULL) {
        head->print(os);
    }
    if (getBodySize() > 0) {
        os << " :- \n   " ;
        os << join(getBodyLiterals(), ",\n   ", print_deref<AstLiteral*>());
    }
    os << ".";
    if (getExecutionPlan()) {
        getExecutionPlan()->print(os);
    }
}

void AstClause::reorderAtoms(const std::vector<unsigned int> &newOrder) {
    // Validate given order
    assert(newOrder.size() == atoms.size());
    std::vector<unsigned int> nopOrder;
    for (unsigned int i = 0; i < atoms.size(); i++) {
        nopOrder.push_back(i);
    }
    assert(std::is_permutation(nopOrder.begin(), nopOrder.end(), newOrder.begin()));

    // Reorder atoms
    std::vector<std::unique_ptr<AstAtom>> oldAtoms(atoms.size());
    atoms.swap(oldAtoms);
    for (unsigned int i = 0; i < newOrder.size(); i++) {
        atoms[i].swap(oldAtoms[newOrder[i]]);
    }
}

} // end of namespace souffle

