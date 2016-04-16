/*
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All Rights reserved
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
 * @file AstUtils.cpp
 *
 * A collection of utilities operating on AST constructs.
 *
 ***********************************************************************/

#include "AstUtils.h"
#include "AstVisitor.h"

#include "Constraints.h"
#include "Util.h"

#include "TypeSystem.h"

namespace souffle {

std::vector<const AstVariable*> getVariables(const AstNode& root) {
    // simply collect the list of all variables by visiting all variables
    std::vector<const AstVariable*> vars;
    visitDepthFirst(root, [&](const AstVariable& var) {
            vars.push_back(&var);
            });
    return vars;
}

std::vector<const AstVariable*> getVariables(const AstNode* root) {
    return getVariables(*root);
}

const AstRelation *getAtomRelation(const AstAtom *atom, const AstProgram *program) {
    return program->getRelation(atom->getName());
}

const AstRelation *getHeadRelation(const AstClause *clause, const AstProgram *program) {
    return getAtomRelation(clause->getHead(), program);
}

std::set<const AstRelation *> getBodyRelations(const AstClause *clause, const AstProgram *program) {
    std::set<const AstRelation *> bodyRelations;
    for(const auto& lit : clause->getBodyLiterals()) {
        visitDepthFirst(*lit, [&](const AstAtom& atom) {
            bodyRelations.insert(getAtomRelation(&atom, program));
        });
    }
    for(const auto& arg : clause->getHead()->getArguments()) {
        visitDepthFirst(*arg, [&](const AstAtom& atom) {
            bodyRelations.insert(getAtomRelation(&atom, program));
        });
    }
    return bodyRelations;
}

bool hasClauseWithNegatedRelation(const AstRelation *relation, const AstRelation *negRelation, const AstProgram *program, const AstLiteral *&foundLiteral) {
    for(const AstClause* cl : relation->getClauses()) {
        for(const AstNegation* neg : cl->getNegations()) {
            if (negRelation == getAtomRelation(neg->getAtom(), program)) {
                foundLiteral = neg;
                return true;
            }
        }
    }
    return false;
}

bool hasClauseWithAggregatedRelation(const AstRelation *relation, const AstRelation *aggRelation, const AstProgram *program, const AstLiteral *&foundLiteral) {
    for(const AstClause* cl : relation->getClauses()) {
        bool hasAgg = false;
        visitDepthFirst(*cl, [&](const AstAggregator& cur){
            visitDepthFirst(cur, [&](const AstAtom& atom) {
                if (aggRelation == getAtomRelation(&atom, program)) {
                    foundLiteral = &atom;
                    hasAgg = true;
                }
            });
        });
        if (hasAgg) {
            return true;
        }
    }
    return false;
}

} // end of namespace souffle

