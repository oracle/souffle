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

namespace souffle {

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

} // end of namespace souffle

