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

    std::vector<size_t> sccColor;

   /*
    * Compute strongly connected components using Gabow's algorithm (cf. Algorithms in
    * Java by Robert Sedgewick / Part 5 / Graph *  algorithms). The algorithm has linear
    * runtime.
    */
    template <template <typename> class Table, typename Node, typename Compare = std::less<Node>>
    static void toSCCGraphRecursive(const Graph<Node, Compare>& graph, HyperGraph<Table, Node>& sccGraph, const Node& w, std::map<Node, int>& preOrder, size_t& counter, std::stack<Node>& S, std::stack<Node>& P) {

        preOrder[w] = counter++;

        S.push(w);
        P.push(w);

        for(const Node& t : graph.getPredecessors(w))
            if (preOrder[t] == -1)
                toSCCGraphRecursive(graph, sccGraph, t, preOrder, counter, S, P);
            else if (!sccGraph.vertexTable().has(t))
                while (preOrder[P.top()] > preOrder[t])
                    P.pop();

        if (P.top() == w)
            P.pop();
        else
            return;

        Node v;
        size_t s = sccGraph.vertexCount();
        sccGraph.insertVertex(s);
        do {
            v = S.top();
            S.pop();
            sccGraph.appendToVertex(s, v);
        } while(v != w);
    }


    template <template <typename> class Table, typename Node, typename Compare = std::less<Node>>
    static HyperGraph<Table, Node> toSCCGraph(const Graph<Node, Compare> graph) {
            size_t counter = 0;
            std::stack<Node> S, P;
            std::map<Node, int> preOrder;
            HyperGraph<Table, Node> sccGraph = HyperGraph<Table, Node>();
            for (const Node& vertex : graph.allVertices())
                   preOrder[vertex] = -1;
            for (const Node& vertex : graph.allVertices())
                if (preOrder[vertex]  == -1)
                    toSCCGraphRecursive(graph, sccGraph, vertex, preOrder, counter, S, P);
            for (const Node& vertex : graph.allVertices())
                for (const Node& predecessor : graph.getPredecessors(vertex))
                    if (vertex != predecessor && sccGraph.vertexTable().getIndex(vertex) != sccGraph.vertexTable().getIndex(predecessor))
                        sccGraph.insertEdge(sccGraph.vertexTable().getIndex(vertex), sccGraph.vertexTable().getIndex(predecessor));
            return sccGraph;
    }

public:

     const HyperGraph<index::SetTable, const AstRelation*>& getGraph() {
        return sccGraph;
     }

    static constexpr const char* name = "scc-graph";

    virtual void run(const AstTranslationUnit& translationUnit) {
        precedenceGraph = translationUnit.getAnalysis<PrecedenceGraph>();
        sccGraph = toSCCGraph<index::SetTable>(precedenceGraph->getGraph());
        sccColor.resize(getNumSCCs());
        std::fill(sccColor.begin(), sccColor.end(), 0);
    }

    size_t getSCCForRelation(const AstRelation* relation) {
        return sccGraph.vertexTable().getIndex(relation);
        // return nodeToSCC[relation];
    }

    /** Get all successor SCCs of a specified scc. */
    const std::set<size_t>& getSuccessorSCCs(size_t scc) {
        return sccGraph.getSuccessors(scc);
        // return succSCC[scc];
    }

    /** Get all predecessor SCCs of a specified scc. */
    const std::set<size_t>& getPredecessorSCCs(size_t scc) {
        return sccGraph.getPredecessors(scc);
        // return predSCC[scc];
    }

    const std::set<const AstRelation*> getRelationsForSCC(size_t scc) {
        return sccGraph.vertexTable().get(scc);
        // return SCC[scc];
    }

    /** Return the number of strongly connected components in the SCC graph */
    size_t getNumSCCs() {
        return sccGraph.vertexCount();
        // return succSCC.size();
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

    /** Get the color of an SCC. */
    const size_t getColor(const size_t scc) {
        return sccColor[scc];
    }

    /** Set the color of an SCC. */
    void setColor(const size_t scc, const size_t color) {
        sccColor[scc] = color;
    }

    /** Fill all SCCs to the given color. */
    void fillColors(const size_t color) {
        std::fill(sccColor.begin(), sccColor.end(), color);
    }

    /** Check if a given SCC has a predecessor of the specified color. */
    const bool hasPredecessorOfColor(size_t scc, const size_t color) {
        for (auto pred : getPredecessorSCCs(scc))
            if (getColor(pred) == color) return true;
        return false;
    }

    /** Check if a given SCC has a successor of the specified color. */
    const bool hasSuccessorOfColor(size_t scc, const size_t color) {
        for (auto succ : getSuccessorSCCs(scc))
            if (getColor(succ) == color) return true;
        return false;
    }


    /** Output strongly connected component graph in graphviz format */
    void outputSCCGraph(std::ostream& os) {
        getGraph().print(os);
    }

};

///**
// * Analysis pass computing a topologically sorted strongly connected component (SCC) graph.
// */
//class TopologicallySortedSCCGraph : public AstAnalysis {
//private:
//    /** The strongly connected component (SCC) graph. */
//    SCCGraph* sccGraph;

//    /** The topological ordering of the SCCs. */
//    std::vector<size_t> orderedSCCs;

//    /** The cost of the topological ordering. */
//    template <template <typename> class Table, typename Node>
//    const int orderCost(HyperGraph<Table, Node> graph,
//            const std::vector<size_t>& permutationOfSCCs) const {
//        // create variables to hold the cost of the current SCC and the permutation as a whole
//        int costOfSCC = 0;
//        int costOfPermutation = -1;
//        // obtain an iterator to the end of the already ordered partition of sccs
//        auto it_k = permutationOfSCCs.begin() + orderedSCCs.size();
//        // for each of the scc's in the ordering, resetting the cost of the scc to zero on each loop
//        for (auto it_i = permutationOfSCCs.begin(); it_i != permutationOfSCCs.end(); ++it_i, costOfSCC = 0) {
//            // if the index of the current scc is after the end of the ordered partition
//            if (it_i >= it_k)
//                // check that the index of all predecessor sccs of are before the index of the current scc
//                for (auto scc : sccGraph->getGraph().getPredecessors(*it_i))
//                    if (std::find(permutationOfSCCs.begin(), it_i, scc) == it_i)
//                        // if not, the sort is not a valid topological sort
//                        return -1;
//            // otherwise, calculate the cost of the current scc
//            // as the number of sccs with an index before the current scc
//            for (auto it_j = permutationOfSCCs.begin(); it_j != it_i; ++it_j)
//                // having some successor scc with an index after the current scc
//                for (auto scc : sccGraph->getGraph().getSuccessors(*it_j))
//                    if (std::find(permutationOfSCCs.begin(), it_i, scc) == it_i) costOfSCC++;
//            // and if this cost is greater than the maximum recorded cost for the whole permutation so far,
//            // set the cost of the permutation to it
//            if (costOfSCC > costOfPermutation) costOfPermutation = costOfSCC;
//        }
//        return costOfPermutation;
//    }


//public:
//    static constexpr const char* name = "topological-scc-graph";

//    virtual void run(const AstTranslationUnit& translationUnit) {
//        sccGraph = translationUnit.getAnalysis<SCCGraph>();
//        orderedSCCs = GraphOrder::innerOrder(preProcessGraph(sccGraph->getGraph()), &GraphSearch::khansAlgorithm);
//    }

//    SCCGraph* getSCCGraph() const {
//        return sccGraph;
//    }

//    const std::vector<size_t>& getSCCOrder() const {
//        return orderedSCCs;
//    }

//    /** Output topologically sorted strongly connected component graph in text format */
//    void outputTopologicallySortedSCCGraph(std::ostream& os) const {
//        for (size_t i = 0; i < orderedSCCs.size(); i++)
//            os << "[" << join(sccGraph->getGraph().vertexTable().get(orderedSCCs[i])) << "]\n";
//        os << "\n";
//        os << "cost: " << orderCost(sccGraph->getGraph(), orderedSCCs) << "\n";
//    }
//};


/**
 * Analysis pass computing a topologically sorted strongly connected component (SCC) graph.
 */
class TopologicallySortedSCCGraph : public AstAnalysis {
private:
    /** The strongly connected component (SCC) graph. */
    SCCGraph* sccGraph;

    /** The final topological ordering of the SCCs. */
    std::vector<size_t> orderedSCCs;

    /** Marker type to compute topological ordering. */
    enum Colour { WHITE = 0xFFFFFF, GRAY = 0x7f7f7f, BLACK = 0x000000 };

    /** Calculate the topological ordering cost of a permutation of as of yet unordered SCCs
    using the ordered SCCs. Returns -1 if the given vector is not a valid topological ordering. */
    const int topologicalOrderingCost(const std::vector<size_t>& permutationOfSCCs) const;

    /** Recursive component for the forwards algorithm computing the topological ordering of the SCCs. */
    void forwardAlgorithmRecursive(int scc);

    /* Forwardss algorithm for computing the topological order of SCCs, based on Khan's algorithm. */
    void forwardAlgorithm();


    /** Pre-process the SCC graph; recursively contract roots, contract leaves, and smooth vertices of out
     * degree 1.  */
    HyperGraph<index::SeqTable, size_t> preProcessGraph(HyperGraph<index::SetTable, const AstRelation*> originalGraph) const {
        HyperGraph<index::SeqTable, size_t> indexGraph = HyperGraph<index::SeqTable, size_t>::toHyperGraph(originalGraph);
        return indexGraph;
        /*
        bool flag = true;
        int in, out, non = -1;
        while (flag) {
            flag = false;
            for (size_t vertex : indexGraph.allVertices()) {
                if (!indexGraph.hasVertex(vertex)) continue;
                in = indexGraph.getPredecessors(vertex).size();
                out = indexGraph.getSuccessors(vertex).size();
                if (in == 0 && out == 0 && (non < 0 || vertex != (size_t) non)) {
                    if (non < 0)
                        non = vertex;
                    else
                        indexGraph.joinVertices(non, vertex);
                    flag = true;
                    continue;
                } else
                if (in == 1 && out == 0) {
                    indexGraph.joinVertices(*indexGraph.getPredecessors(vertex).begin(), vertex);
                    flag = true;
                    continue;
                } else
                if (out == 1) {
                    indexGraph.joinVertices(*indexGraph.getSuccessors(vertex).begin(), vertex);
                    flag = true;
                    continue;
                }
            }
        }

        return indexGraph;
        */

    }

public:


    static constexpr const char* name = "topological-scc-graph";

    virtual void run(const AstTranslationUnit& translationUnit);

    SCCGraph* getSCCGraph() {
        return sccGraph;
    }

    const std::vector<size_t>& getSCCOrder() {
        return orderedSCCs;
    }

    /** Output topologically sorted strongly connected component graph in text format */
    void outputTopologicallySortedSCCGraph(std::ostream& os);
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
