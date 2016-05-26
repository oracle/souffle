/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
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

