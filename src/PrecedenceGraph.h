/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file PrecedenceGraph.cpp
 *
 * Defines the class for precedence graph to build the precedence graph,
 * compute strongly connected components of the precedence graph, and
 * build the strongly connected component graph.
 *
 ***********************************************************************/

#pragma once

#include "AstAnalysis.h"
#include "AstProgram.h"
#include "AstTranslationUnit.h"
#include "GraphUtils.h"

#include <iomanip>
#include <list>
#include <map>
#include <stack>
#include <string>
#include <vector>

namespace souffle {

typedef Graph<const AstRelation*, AstNameComparison> AstRelationGraph;

/**
 * Analysis pass computing the precedence graph of the relations of the datalog progam.
 */
class PrecedenceGraph : public AstAnalysis {
private:
    /** Adjacency list of precedence graph (determined by the dependencies of the relations) */
    AstRelationGraph precedenceGraph;

public:
    static constexpr const char* name = "precedence-graph";

    virtual void run(const AstTranslationUnit& translationUnit);

    /** Output precedence graph in graphviz format to a given stream */
    void outputPrecedenceGraph(std::ostream& os);

    const AstRelationSet& getPredecessors(const AstRelation* relation) {
        assert(precedenceGraph.hasVertex(relation) && "Relation not present in precedence graph!");
        return precedenceGraph.getSuccessors(relation);
    }

    const AstRelationGraph getGraph() const {
        return precedenceGraph;
    }
};

/**
 * Analysis pass identifying relations which do not contribute to the computation
 * of the output relations.
 */
class RedundantRelations : public AstAnalysis {
private:
    PrecedenceGraph* precedenceGraph;

    std::set<const AstRelation*> redundantRelations;

public:
    static constexpr const char* name = "redundant-relations";

    virtual void run(const AstTranslationUnit& translationUnit);

    const std::set<const AstRelation*>& getRedundantRelations() {
        return redundantRelations;
    }
};

/**
 * Analysis pass identifying clauses which are recursive.
 */
class RecursiveClauses : public AstAnalysis {
private:
    std::set<const AstClause*> recursiveClauses;

    /** Determines whether the given clause is recursive within the given program */
    bool computeIsRecursive(const AstClause& clause, const AstTranslationUnit& translationUnit) const;

public:
    static constexpr const char* name = "recursive-clauses";

    virtual void run(const AstTranslationUnit& translationUnit);

    bool isRecursive(const AstClause* clause) const {
        return recursiveClauses.count(clause);
    }
};

/**
 * Analysis pass computing the strongly connected component (SCC) graph for the datalog program.
 */
class SCCGraph : public AstAnalysis {

private:
    PrecedenceGraph* precedenceGraph;

    HyperGraph<index::SetTable, const AstRelation*> sccGraph;


public:

     const HyperGraph<index::SetTable, const AstRelation*>& getGraph() {
        return sccGraph;
     }

    static constexpr const char* name = "scc-graph";

    virtual void run(const AstTranslationUnit& translationUnit) {
        precedenceGraph = translationUnit.getAnalysis<PrecedenceGraph>();
        sccGraph = GraphUtils::toSCCGraph<index::SetTable>(precedenceGraph->getGraph());
    }

    size_t getSCCForRelation(const AstRelation* relation) {
        return sccGraph.vertexTable().getIndex(relation);
    }

    /** Get all successor SCCs of a specified scc. */
    const std::set<size_t>& getSuccessorSCCs(size_t scc) {
        return sccGraph.getSuccessors(scc);
    }

    /** Get all predecessor SCCs of a specified scc. */
    const std::set<size_t>& getPredecessorSCCs(size_t scc) {
        return sccGraph.getPredecessors(scc);
    }

    const std::set<const AstRelation*> getRelationsForSCC(size_t scc) {
        return sccGraph.vertexTable().get(scc);
    }

    /** Return the number of strongly connected components in the SCC graph */
    size_t getNumSCCs() {
        return sccGraph.vertexCount();
    }

    bool isRecursive(size_t scc) {
        const std::set<const AstRelation*>& sccRelations = getRelationsForSCC(scc);
        if (sccRelations.size() == 1) {
            const AstRelation* singleRelation = *sccRelations.begin();
            if (!precedenceGraph->getPredecessors(singleRelation).count(singleRelation)) {
                return false;
            }
        }
        return true;
    }

    bool isRecursive(const AstRelation* relation) {
        return isRecursive(getSCCForRelation(relation));
    }

    /** Output strongly connected component graph in graphviz format */
    void outputSCCGraph(std::ostream& os) {
        getGraph().print(os);
    }

};


/**
 * Analysis pass computing a topologically sorted strongly connected component (SCC) graph.
 */
class TopologicallySortedSCCGraph : public AstAnalysis {
private:
    /** The strongly connected component (SCC) graph. */
    SCCGraph* sccGraph;

    /** The final topological ordering of the SCCs. */
    std::vector<size_t> orderedSCCs;




const int topologicalOrderingCost(
        const std::vector<size_t>& permutationOfSCCs) const {
    // create variables to hold the cost of the current SCC and the permutation as a whole
    int costOfSCC = 0;
    int costOfPermutation = -1;
    // obtain an iterator to the end of the already ordered partition of sccs
    auto it_k = permutationOfSCCs.begin() + orderedSCCs.size();
    // for each of the scc's in the ordering, resetting the cost of the scc to zero on each loop
    for (auto it_i = permutationOfSCCs.begin(); it_i != permutationOfSCCs.end(); ++it_i, costOfSCC = 0) {
        // if the index of the current scc is after the end of the ordered partition
        if (it_i >= it_k)
            // check that the index of all predecessor sccs of are before the index of the current scc
            for (int scc : sccGraph->getPredecessorSCCs(*it_i))
                if (std::find(permutationOfSCCs.begin(), it_i, scc) == it_i)
                    // if not, the sort is not a valid topological sort
                    return -1;
        // otherwise, calculate the cost of the current scc
        // as the number of sccs with an index before the current scc
        for (auto it_j = permutationOfSCCs.begin(); it_j != it_i; ++it_j)
            // having some successor scc with an index after the current scc
            for (int scc : sccGraph->getSuccessorSCCs(*it_j))
                if (std::find(permutationOfSCCs.begin(), it_i, scc) == it_i) costOfSCC++;
        // and if this cost is greater than the maximum recorded cost for the whole permutation so far,
        // set the cost of the permutation to it
        if (costOfSCC > costOfPermutation) {
            costOfPermutation = costOfSCC;
        }
    }
    return costOfPermutation;
}



public:


    static constexpr const char* name = "topological-scc-graph";


virtual void run(const AstTranslationUnit& translationUnit) {
    // obtain the scc graph
    sccGraph = translationUnit.getAnalysis<SCCGraph>();
    // generate topological ordering using forwards algorithm (like Khan's algorithm)

    orderedSCCs = GraphUtils::khansAlgorithm(sccGraph->getGraph());
    // orderedSCCs = khansAlgorithm(HyperGraph<index::SeqTable, size_t>::toHyperGraph(sccGraph->getGraph()));
    // orderedSCCs = khansAlgorithm(preProcessGraph(sccGraph->getGraph()));
    // TODO

    // orderedSCCs = GraphOrder::innerOrder(preProcessGraph(sccGraph->getGraph()), &GraphSearch::khansAlgorithm);

}

    SCCGraph* getSCCGraph() {
        return sccGraph;
    }

    const std::vector<size_t>& getSCCOrder() {
        return orderedSCCs;
    }


void outputTopologicallySortedSCCGraph(std::ostream& os) {
//        for (size_t i = 0; i < orderedSCCs.size(); i++)
//            os << "[" << join(sccGraph->getGraph().vertexTable().get(orderedSCCs[i])) << "]\n";
//        os << "\n";
//        os << "cost: " << orderCost(sccGraph->getGraph(), orderedSCCs) << "\n";
    int numSCCs = orderedSCCs.size();
    for (int i = 0; i < numSCCs; i++) {
        os << "[";
        os << join(sccGraph->getRelationsForSCC(orderedSCCs[i]), ", ",
                [](std::ostream& out, const AstRelation* rel) { out << rel->getName(); });
        os << "]\n";
    }
    os << "\n";
    os << "cost: " << topologicalOrderingCost(orderedSCCs) << "\n";
}

};

/**
 * A single step in a relation schedule, consisting of the relations computed in the step
 * and the relations that are no longer required at that step.
 */
class RelationScheduleStep {
private:
    std::set<const AstRelation*> computedRelations;
    std::set<const AstRelation*> expiredRelations;
    const bool recursive;

public:
    RelationScheduleStep(std::set<const AstRelation*> computedRelations,
            std::set<const AstRelation*> expiredRelations, const bool recursive)
            : computedRelations(computedRelations), expiredRelations(expiredRelations), recursive(recursive) {
    }

    const std::set<const AstRelation*>& getComputedRelations() const {
        return computedRelations;
    }

    const std::set<const AstRelation*>& getExpiredRelations() const {
        return expiredRelations;
    }

    bool isRecursive() const {
        return recursive;
    }
};

/**
 * Analysis pass computing a schedule for computing relations.
 */
class RelationSchedule : public AstAnalysis {
private:
    TopologicallySortedSCCGraph* topsortSCCGraph;
    PrecedenceGraph* precedenceGraph;

    /** Relations computed and expired relations at each step */
    std::vector<RelationScheduleStep> schedule;

    std::vector<std::set<const AstRelation*>> computeRelationExpirySchedule(
            const AstTranslationUnit& translationUnit);

public:
    static constexpr const char* name = "relation-schedule";

    virtual void run(const AstTranslationUnit& translationUnit);

    const std::vector<RelationScheduleStep>& getSchedule() {
        return schedule;
    }

    bool isRecursive(const AstRelation* relation) {
        return topsortSCCGraph->getSCCGraph()->isRecursive(relation);
    }

    void dump() {
        std::cerr << "begin schedule\n";
        for (RelationScheduleStep& step : schedule) {
            std::cerr << "computed: ";
            for (const AstRelation* compRel : step.getComputedRelations()) {
                std::cerr << compRel->getName() << ", ";
            }
            std::cerr << "\nexpired: ";
            for (const AstRelation* compRel : step.getExpiredRelations()) {
                std::cerr << compRel->getName() << ", ";
            }
            std::cerr << "\n";
            if (step.isRecursive()) {
                std::cerr << "recursive";
            } else {
                std::cerr << "not recursive";
            }
            std::cerr << "\n";
        }
        std::cerr << "end schedule\n";
    }
};

}  // end of namespace souffle
