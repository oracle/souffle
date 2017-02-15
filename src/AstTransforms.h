/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file AstTransforms.h
 *
 * Defines AST transformation passes.
 *
 ***********************************************************************/
#pragma once

#include "AstTransformer.h"
#include "AstTranslationUnit.h"

namespace souffle {

/**
 * Transformation pass to eliminate grounded aliases.
 * e.g. resolve  a(r) , r = [x,y]    =>    a(x,y)
 */
class ResolveAliasesTransformer : public AstTransformer {
private:
    virtual bool transform(AstTranslationUnit& translationUnit) {
        resolveAliases(*translationUnit.getProgram());
        return true;
    }
    static void removeComplexTermsInAtoms(AstClause& clause);

public:
    virtual std::string getName() const {
        return "ResolveAliasesTransformer";
    }

    /**
     * Converts the given clause into a version without variables aliasing
     * grounded variables.
     *
     * @param clause the clause to be processed
     * @return a clone of the processed clause
     */
    static std::unique_ptr<AstClause> resolveAliases(const AstClause& clause);

    /**
     * Removes trivial equalities of the form t = t from the given clause.
     *
     * @param clause the clause to be processed
     * @return a modified clone of the given clause
     */
    static std::unique_ptr<AstClause> removeTrivialEquality(const AstClause& clause);

    /**
     * Eliminate grounded aliases in the given program.
     *
     * @param program the program to be processed
     */
    static void resolveAliases(AstProgram& program);
};

/**
 * Transformation pass to replaces copy of relations by their origin.
 * For instance, if a relation r is defined by
 *
 *      r(X,Y) :- s(X,Y)
 *
 * and no other clause, all occurrences of r will be replaced by s.
 */
class RemoveRelationCopiesTransformer : public AstTransformer {
private:
    virtual bool transform(AstTranslationUnit& translationUnit) {
        return removeRelationCopies(*translationUnit.getProgram());
    }

public:
    virtual std::string getName() const {
        return "RemoveRelationCopiesTransformer";
    }

    /**
     * Replaces copies of relations by their origin in the given program.
     *
     * @param program the program to be processed
     * @return whether the program was modified
     */
    static bool removeRelationCopies(AstProgram& program);
};

/**
 * Transformation pass to rename aggregation variables to make them unique.
 */
class UniqueAggregationVariablesTransformer : public AstTransformer {
private:
    virtual bool transform(AstTranslationUnit& translationUnit);

public:
    virtual std::string getName() const {
        return "UniqueAggregationVariablesTransformer";
    }
};

/**
 * Transformation pass to create artificial relations for bodies of
 * aggregation functions consisting of more than a single atom.
 */
class MaterializeAggregationQueriesTransformer : public AstTransformer {
private:
    virtual bool transform(AstTranslationUnit& translationUnit) {
        return materializeAggregationQueries(translationUnit);
    }

    /**
     * A test determining whether the body of a given aggregation needs to be
     * 'outlined' into an independent relation or can be kept inline.
     */
    static bool needsMaterializedRelation(const AstAggregator& agg);

public:
    virtual std::string getName() const {
        return "MaterializeAggregationQueriesTransformer";
    }

    /**
     * Creates artificial relations for bodies of aggregation functions
     * consisting of more than a single atom, in the given program.
     *
     * @param program the program to be processed
     * @return whether the program was modified
     */
    static bool materializeAggregationQueries(AstTranslationUnit& translationUnit);
};

/**
 * Transformation pass to remove all empty relations and rules that use empty relations.
 */
class RemoveEmptyRelationsTransformer : public AstTransformer {
private:
    virtual bool transform(AstTranslationUnit& translationUnit) {
        return removeEmptyRelations(translationUnit);
    }

    /**
     * Eliminate rules that contain empty relations and/or rewrite them.
     *
     * @param translationUnit the program to be processed
     * @param emptyRelation relation that is empty
     */
    static void removeEmptyRelationUses(AstTranslationUnit& translationUnit, AstRelation* emptyRelation);

public:
    virtual std::string getName() const {
        return "RemoveEmptyRelationsTransformer";
    }

    /**
     * Eliminate all empty relations (and their uses) in the given program.
     *
     * @param translationUnit the program to be processed
     * @return whether the program was modified
     */
    static bool removeEmptyRelations(AstTranslationUnit& translationUnit);
};

/**
 * Transformation pass to remove relations which are redundant (do not contribute to output).
 */
class RemoveRedundantRelationsTransformer : public AstTransformer {
private:
    virtual bool transform(AstTranslationUnit& translationUnit);

public:
    virtual std::string getName() const {
        return "RemoveRedundantRelationsTransformer";
    }
};

}  // end of namespace souffle
