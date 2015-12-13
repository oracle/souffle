/*
 * Copyright (c) 2013-15, Oracle and/or its affiliates.
 *
 * All rights reserved.
 */

/************************************************************************
 *
 * @file RamTranslator.h
 *
 * Defines utilities for translating AST structures into RAM constructs.
 *
 ***********************************************************************/

#pragma once

#include <vector>
#include <map>

#include "RamRelation.h"
#include "AstTranslationUnit.h"

// forward declarations
class AstRelation;
class AstAtom;
class AstClause;
class AstProgram;

class RamStatement;

class RecursiveClauses;


/**
 * A utility class capable of conducting the conversion between AST
 * and RAM structures.
 */
class RamTranslator {

    /** If true, created constructs will be annotated with logging information */
    bool logging;

public:

    /**
     * A constructor for this translators.
     *
     * @param logging if generated clauses should be annotated with logging operations
     */
    RamTranslator(bool logging = false) : logging(logging) {}

    /**
     * Converts the given relation identifier into a relation name.
     */
    std::string translateRelationName(const AstRelationIdentifier& id);

    /** generate RAM code for a clause */
    std::unique_ptr<RamStatement> translateClause(const AstClause& clause, const AstProgram *program, const TypeEnvironment *typeEnv, int version = 0);

    /**
     * Generates RAM code for the non-recursive clauses of the given relation.
     *
     * @return a corresponding statement or null if there are no non-recursive clauses.
     */
    std::unique_ptr<RamStatement> translateNonRecursiveRelation(const AstRelation& rel, const AstProgram *program, const RecursiveClauses *recursiveClauses, const TypeEnvironment &typeEnv);

    /** generate RAM code for recursive relations in a strongly-connected component */
    std::unique_ptr<RamStatement> translateRecursiveRelation(const std::set<const AstRelation*> &scc, const AstProgram *program, const RecursiveClauses *recursiveClauses, const TypeEnvironment &typeEnv);

    /** translates the given datalog program into an equivalent RAM program  */
    std::unique_ptr<RamStatement> translateProgram(const AstTranslationUnit &translationUnit);

};
