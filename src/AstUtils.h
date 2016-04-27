/*
 * Souffle version 0.0.0
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - souffle/LICENSE
 */

/************************************************************************
 *
 * @file AstUtils.h
 *
 * A collection of utilities operating on AST constructs.
 *
 ***********************************************************************/

#pragma once

#include <map>
#include <vector>
#include <set>

namespace souffle {

// some forward declarations
class AstNode;
class AstVariable;
class AstRelation;
class AstAtom;
class AstClause;
class AstProgram;
class AstLiteral;


// ---------------------------------------------------------------
//                      General Utilities
// ---------------------------------------------------------------


/**
 * Obtains a list of all variables referenced within the AST rooted
 * by the given root node.
 *
 * @param root the root of the AST to be searched
 * @return a list of all variables referenced within
 */
std::vector<const AstVariable*> getVariables(const AstNode& root);

/**
 * Obtains a list of all variables referenced within the AST rooted
 * by the given root node.
 *
 * @param root the root of the AST to be searched
 * @return a list of all variables referenced within
 */
std::vector<const AstVariable*> getVariables(const AstNode* root);

/**
 * Returns the relation referenced by the given atom.
 * @param atom the atom
 * @param program the program containing the relations
 * @return relation referenced by the atom
 */
const AstRelation *getAtomRelation(const AstAtom *atom, const AstProgram *program);

/**
 * Returns the relation referenced by the head of the given clause.
 * @param clause the clause
 * @param program the program containing the relations
 * @return relation referenced by the clause head
 */
const AstRelation *getHeadRelation(const AstClause *clause, const AstProgram *program);

/**
 * Returns the relations referenced in the body of the given clause.
 * @param clause the clause
 * @param program the program containing the relations
 * @return relation referenced in the clause body
 */
std::set<const AstRelation *> getBodyRelations(const AstClause *clause, const AstProgram *program);

/**
 * Returns whether the given relation has any clauses which contain a negation of a specific relation.
 * @param relation the relation to search the clauses of
 * @param negRelation the relation to search for negations of in clause bodies
 * @param program the program containing the relations
 * @param foundLiteral set to the negation literal that was found
 */
bool hasClauseWithNegatedRelation(const AstRelation *relation, const AstRelation *negRelation, const AstProgram *program, const AstLiteral *&foundLiteral);

/**
 * Returns whether the given relation has any clauses which contain an aggregation over of a specific relation.
 * @param relation the relation to search the clauses of
 * @param aggRelation the relation to search for in aggregations in clause bodies
 * @param program the program containing the relations
 * @param foundLiteral set to the literal found in an aggregation
 */
bool hasClauseWithAggregatedRelation(const AstRelation *relation, const AstRelation *aggRelation, const AstProgram *program, const AstLiteral *&foundLiteral);

} // end of namespace souffle

