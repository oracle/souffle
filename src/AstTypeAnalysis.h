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
 * @file AstTypeAnalysis.h
 *
 * A collection of type analyses operating on AST constructs.
 *
 ***********************************************************************/
#pragma once

#include "TypeSystem.h"
#include "AstTranslationUnit.h"
#include "AstAnalysis.h"

namespace souffle {

class AstNode;
class AstProgram;
class AstArgument;
class AstClause;
class AstVariable;
class AstRelation;

/**
 * Analyse the given clause and computes for each contained argument
 * whether it is a constant value or not.
 *
 * @param clause the clause to be analyzed
 * @return a map mapping each contained argument to a boolean indicating
 *      whether the argument represents a constant value or not
 */
std::map<const AstArgument*, bool> getConstTerms(const AstClause& clause);

/**
 * Analyse the given clause and computes for each contained argument
 * whether it is a grounded value or not.
 *
 * @param clause the clause to be analyzed
 * @return a map mapping each contained argument to a boolean indicating
 *      whether the argument represents a grounded value or not
 */
std::map<const AstArgument*, bool> getGroundedTerms(const AstClause& clause);

class TypeEnvironmentAnalysis : public AstAnalysis {
private:
    TypeEnvironment env;

    void updateTypeEnvironment(const AstProgram &program);
public:
    static constexpr const char *name = "type-environment";

    virtual void run(const AstTranslationUnit &translationUnit);

    const TypeEnvironment &getTypeEnvironment() {
        return env;
    }
};

class TypeAnalysis : public AstAnalysis {
private:
    std::map<const AstArgument *, TypeSet> argumentTypes;

public:
    static constexpr const char *name = "type-analysis";

    virtual void run(const AstTranslationUnit &translationUnit);

    /**
     * Get the computed types for the given argument.
     */
    TypeSet getTypes(const AstArgument *argument) const {
        auto found = argumentTypes.find(argument);
        assert(found != argumentTypes.end());
        return found->second;
    }

    /**
     * Analyse the given clause and computes for each contained argument
     * a set of potential types. If the set associated to an argument is empty,
     * no consistent typing can be found and the rule can not be properly typed.
     *
     * @param env a typing environment describing the set of available types
     * @param clause the clause to be typed
     * @param program the program
     * @return a map mapping each contained argument to a a set of types
     */
    static std::map<const AstArgument*, TypeSet> analyseTypes(const TypeEnvironment& env, const AstClause& clause, const AstProgram *program, bool verbose = false);
};

} // end of namespace souffle


