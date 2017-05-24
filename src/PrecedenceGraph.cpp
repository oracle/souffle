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
 * Implements method of precedence graph to build the precedence graph,
 * compute strongly connected components of the precedence graph, and
 * build the strongly connected component graph.
 *
 ***********************************************************************/

#include "PrecedenceGraph.h"
#include "AstClause.h"
#include "AstRelation.h"
#include "AstUtils.h"
#include "AstVisitor.h"

#include <algorithm>
#include <set>

namespace souffle {

void PrecedenceGraph::run(const AstTranslationUnit& translationUnit) {
    /* Get relations */
    std::vector<AstRelation*> relations = translationUnit.getProgram()->getRelations();

    for (AstRelation* r : relations) {
        precedenceGraph.addNode(r);
        for (size_t i = 0; i < r->clauseSize(); i++) {
            AstClause* c = r->getClause(i);
            const std::set<const AstRelation*>& dependencies =
                    getBodyRelations(c, translationUnit.getProgram());
            for (std::set<const AstRelation*>::const_iterator irs = dependencies.begin();
                    irs != dependencies.end(); ++irs) {
                const AstRelation* source = (*irs);
                precedenceGraph.addEdge(r, source);
            }
        }
    }
}

void PrecedenceGraph::outputPrecedenceGraph(std::ostream& os) {
    /* Print dependency graph */
    os << "digraph \"dependence-graph\" {\n";
    /* Print node of dependence graph */
    for (const AstRelation* rel : precedenceGraph.getNodes()) {
        if (rel) {
            os << "\t\"" << rel->getName() << "\" [label = \"" << rel->getName() << "\"];\n";
        }
    }
    for (const AstRelation* rel : precedenceGraph.getNodes()) {
        if (rel) {
            for (const AstRelation* adjRel : precedenceGraph.getEdges(rel)) {
                if (adjRel) {
                    os << "\t\"" << adjRel->getName() << "\" -> \"" << rel->getName() << "\";\n";
                }
            }
        }
    }
    os << "}\n";
}

void RedundantRelations::run(const AstTranslationUnit& translationUnit) {
    precedenceGraph = translationUnit.getAnalysis<PrecedenceGraph>();

    std::set<const AstRelation*> work;
    std::set<const AstRelation*> notRedundant;

    const std::vector<AstRelation*>& relations = translationUnit.getProgram()->getRelations();

    /* Add all output relations to the work set */
    for (const AstRelation* r : relations) {
        if (r->isComputed()) {
            work.insert(r);
        }
    }

    /* Find all relations which are not redundant for the computations of the
       output relations. */
    while (!work.empty()) {
        /* Chose one element in the work set and add it to notRedundant */
        const AstRelation* u = *(work.begin());
        work.erase(work.begin());
        notRedundant.insert(u);

        /* Find all predecessors of u and add them to the worklist
            if they are not in the set notRedundant */
        for (const AstRelation* predecessor : precedenceGraph->getPredecessors(u)) {
            if (!notRedundant.count(predecessor)) {
                work.insert(predecessor);
            }
        }
    }

    /* All remaining relations are redundant. */
    redundantRelations.clear();
    for (const AstRelation* r : relations) {
        if (!notRedundant.count(r)) {
            redundantRelations.insert(r);
        }
    }
}

void RecursiveClauses::run(const AstTranslationUnit& translationUnit) {
    visitDepthFirst(*translationUnit.getProgram(), [&](const AstClause& clause) {
        if (computeIsRecursive(clause, translationUnit)) {
            recursiveClauses.insert(&clause);
        }
    });
}

bool RecursiveClauses::computeIsRecursive(
        const AstClause& clause, const AstTranslationUnit& translationUnit) const {
    const AstProgram& program = *translationUnit.getProgram();

    // we want to reach the atom of the head through the body
    const AstRelation* trg = getHeadRelation(&clause, &program);

    std::set<const AstRelation*> reached;
    std::vector<const AstRelation*> worklist;

    // set up start list
    for (const AstAtom* cur : clause.getAtoms()) {
        auto rel = program.getRelation(cur->getName());
        if (rel == trg) {
            return true;
        }
        worklist.push_back(rel);
    }

    // process remaining elements
    while (!worklist.empty()) {
        // get next to process
        const AstRelation* cur = worklist.back();
        worklist.pop_back();

        // skip null pointers (errors in the input code)
        if (!cur) {
            continue;
        }

        // check whether this one has been checked before
        if (!reached.insert(cur).second) {
            continue;
        }

        // check all atoms in the relations
        for (const AstClause* cl : cur->getClauses()) {
            for (const AstAtom* at : cl->getAtoms()) {
                auto rel = program.getRelation(at->getName());
                if (rel == trg) {
                    return true;
                }
                worklist.push_back(rel);
            }
        }
    }

    // no cycles found
    return false;
}

void SCCGraph::run(const AstTranslationUnit& translationUnit) {
    precedenceGraph = translationUnit.getAnalysis<PrecedenceGraph>();
    SCC.clear();
    nodeToSCC.clear();
    predSCC.clear();
    succSCC.clear();

    /* Compute SCC */
    std::vector<AstRelation*> relations = translationUnit.getProgram()->getRelations();
    unsigned int counter = 0;
    int numSCCs = 0;
    std::stack<const AstRelation *> S, P;
    std::map<const AstRelation*, int> preOrder;  // Pre-order number of a node (for Gabow's Algo)
    for (const AstRelation* relation : relations) {
        nodeToSCC[relation] = preOrder[relation] = -1;
    }
    for (const AstRelation* relation : relations) {
        if (preOrder[relation] == -1) {
            scR(relation, preOrder, counter, S, P, numSCCs);
        }
    }

    /* Build SCC graph */
    succSCC.resize(numSCCs);
    predSCC.resize(numSCCs);
    for (const AstRelation* u : relations) {
        for (const AstRelation* v : precedenceGraph->getPredecessors(u)) {
            int scc_u = nodeToSCC[u];
            int scc_v = nodeToSCC[v];
            ASSERT(scc_u >= 0 && scc_u < numSCCs && "Wrong range");
            ASSERT(scc_v >= 0 && scc_v < numSCCs && "Wrong range");
            if (scc_u != scc_v) {
                predSCC[scc_u].insert(scc_v);
                succSCC[scc_v].insert(scc_u);
            }
        }
    }

    /* Store the relations for each SCC */
    SCC.resize(numSCCs);
    for (const AstRelation* relation : relations) {
        SCC[nodeToSCC[relation]].insert(relation);
    }
}

/* Compute strongly connected components using Gabow's algorithm (cf. Algorithms in
 * Java by Robert Sedgewick / Part 5 / Graph *  algorithms). The algorithm has linear
 * runtime. */
void SCCGraph::scR(const AstRelation* w, std::map<const AstRelation*, int>& preOrder, unsigned int& counter,
        std::stack<const AstRelation*>& S, std::stack<const AstRelation*>& P, int& numSCCs) {
    preOrder[w] = counter++;
    S.push(w);
    P.push(w);
    for (const AstRelation* t : precedenceGraph->getPredecessors(w)) {
        if (preOrder[t] == -1) {
            scR(t, preOrder, counter, S, P, numSCCs);
        } else if (nodeToSCC[t] == -1) {
            while (preOrder[P.top()] > preOrder[t]) {
                P.pop();
            }
        }
    }
    if (P.top() == w) {
        P.pop();
    } else {
        return;
    }

    const AstRelation* v;
    do {
        v = S.top();
        S.pop();
        nodeToSCC[v] = numSCCs;
    } while (v != w);
    numSCCs++;
}

void SCCGraph::outputSCCGraph(std::ostream& os) {
    /* Print SCC graph */
    os << "digraph \"scc-graph\" {\n";
    /* Print nodes of SCC graph */
    int numSCCs = getNumSCCs();
    for (int scc = 0; scc < numSCCs; scc++) {
        os << "\t snode" << scc << "[label = \"";
        os << join(getRelationsForSCC(scc), ",\\n",
                [](std::ostream& out, const AstRelation* rel) { out << rel->getName(); });
        os << "\" ];\n";
    }

    /* Print edges of SCC graph */
    for (int scc = 0; scc < numSCCs; scc++) {
        for (int successor : getSuccessorSCCs(scc)) {
            os << "\tsnode" << scc << " -> snode" << successor << ";\n";
        }
    }
    os << "}\n";
}

const int TopologicallySortedSCCGraph::topologicalOrderingCost(
        const std::vector<int>& permutationOfSCCs) const {
    // create variables to hold the cost of the current SCC and the permutation as a whole
    int costOfSCC = 0;
    int costOfPermutation = -1;
    // obtain an iterator to the end of the already ordered partition of sccs
    auto it_k = permutationOfSCCs.begin() + orderedSCCs.size();
    // for each of the scc's in the ordering, resetting the cost of the scc to zero on each loop
    for (auto it_i = permutationOfSCCs.begin(); it_i != permutationOfSCCs.end(); ++it_i, costOfSCC = 0) {
        // if the index of the current scc is after the end of the ordered partition
        if (it_i >= it_k) {
            // check that the index of all predecessor sccs of are before the index of the current scc
            for (int scc : sccGraph->getPredecessorSCCs(*it_i)) {
                if (std::find(permutationOfSCCs.begin(), it_i, scc) == it_i) {
                    // if not, the sort is not a valid topological sort
                    return -1;
                }
            }
        }
        // otherwise, calculate the cost of the current scc
        // as the number of sccs with an index before the current scc
        for (auto it_j = permutationOfSCCs.begin(); it_j != it_i; ++it_j) {
            // having some successor scc with an index after the current scc
            for (int scc : sccGraph->getSuccessorSCCs(*it_j)) {
                if (std::find(permutationOfSCCs.begin(), it_i, scc) == it_i) {
                    costOfSCC++;
                }
            }
        }
        // and if this cost is greater than the maximum recorded cost for the whole permutation so far,
        // set the cost of the permutation to it
        if (costOfSCC > costOfPermutation) {
            costOfPermutation = costOfSCC;
        }
    }
    return costOfPermutation;
}

void TopologicallySortedSCCGraph::computeTopologicalOrdering(int scc, std::vector<bool>& visited) {
    // create a flag to indicate that a successor was visited (by default it hasn't been)
    bool found = false, hasUnvisitedSuccessor = false, hasUnvisitedPredecessor = false;
    // for each successor of the input scc
    const auto& successorsToVisit = sccGraph->getSuccessorSCCs(scc);
    for (auto scc_i = successorsToVisit.begin(); scc_i != successorsToVisit.end(); ++scc_i) {
        if (visited[*scc_i]) {
            continue;
        }
        hasUnvisitedPredecessor = false;
        const auto& successorsPredecessors = sccGraph->getPredecessorSCCs(*scc_i);
        for (auto scc_j = successorsPredecessors.begin(); scc_j != successorsPredecessors.end(); ++scc_j) {
            if (!visited[*scc_j]) {
                hasUnvisitedPredecessor = true;
                break;
            }
        }
        if (!hasUnvisitedPredecessor) {
            // give it a temporary marking
            visited[*scc_i] = true;
            // add it to the permanent ordering
            orderedSCCs.push_back(*scc_i);
            // and use it as a root node in a recursive call to this function
            computeTopologicalOrdering(*scc_i, visited);
            // finally, indicate that a successor has been found for this node
            found = true;
        }
    }
    // return at once if no valid successors have been found; as either it has none or they all have a
    // better predecessor
    if (!found) {
        return;
    }
    hasUnvisitedPredecessor = false;
    const auto& predecessors = sccGraph->getPredecessorSCCs(scc);
    for (auto scc_j = predecessors.begin(); scc_j != predecessors.end(); ++scc_j) {
        if (!visited[*scc_j]) {
            hasUnvisitedPredecessor = true;
            break;
        }
    }
    hasUnvisitedSuccessor = false;
    const auto& successors = sccGraph->getSuccessorSCCs(scc);
    for (auto scc_j = successors.begin(); scc_j != successors.end(); ++scc_j) {
        if (!visited[*scc_j]) {
            hasUnvisitedSuccessor = true;
            break;
        }
    }
    // otherwise, if more white successors remain for the current scc, use it again as the root node in a
    // recursive call to this function
    if (hasUnvisitedSuccessor && !hasUnvisitedPredecessor) {
        computeTopologicalOrdering(scc, visited);
    }
}

void TopologicallySortedSCCGraph::run(const AstTranslationUnit& translationUnit) {
    // obtain the scc graph
    sccGraph = translationUnit.getAnalysis<SCCGraph>();
    // clear the list of ordered sccs
    orderedSCCs.clear();
    std::vector<bool> visited;
    visited.resize(sccGraph->getNumSCCs());
    std::fill(visited.begin(), visited.end(), false);
    // generate topological ordering using forwards algorithm (like Khan's algorithm)
    // for each of the sccs in the graph
    for (int scc = 0; scc < sccGraph->getNumSCCs(); ++scc) {
        // if that scc has no predecessors
        if (sccGraph->getPredecessorSCCs(scc).empty()) {
            // put it in the ordering
            orderedSCCs.push_back(scc);
            visited[scc] = true;
            // if the scc has successors
            if (!sccGraph->getSuccessorSCCs(scc).empty()) {
                computeTopologicalOrdering(scc, visited);
            }
        }
    }
}

void TopologicallySortedSCCGraph::outputTopologicallySortedSCCGraph(std::ostream& os) {
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

void RelationSchedule::run(const AstTranslationUnit& translationUnit) {
    topsortSCCGraph = translationUnit.getAnalysis<TopologicallySortedSCCGraph>();
    precedenceGraph = translationUnit.getAnalysis<PrecedenceGraph>();

    int numSCCs = topsortSCCGraph->getSCCGraph()->getNumSCCs();
    std::vector<std::set<const AstRelation*>> relationExpirySchedule =
            computeRelationExpirySchedule(translationUnit);

    schedule.clear();
    for (int i = 0; i < numSCCs; i++) {
        int scc = topsortSCCGraph->getSCCOrder()[i];
        const std::set<const AstRelation*> computedRelations =
                topsortSCCGraph->getSCCGraph()->getRelationsForSCC(scc);
        schedule.emplace_back(computedRelations, relationExpirySchedule[i],
                topsortSCCGraph->getSCCGraph()->isRecursive(scc));
    }
}

std::vector<std::set<const AstRelation*>> RelationSchedule::computeRelationExpirySchedule(
        const AstTranslationUnit& translationUnit) {
    std::vector<std::set<const AstRelation*>> relationExpirySchedule;
    /* Compute for each step in the reverse topological order
       of evaluating the SCC the set of alive relations. */

    int numSCCs = topsortSCCGraph->getSCCOrder().size();

    /* Alive set for each step */
    std::vector<std::set<const AstRelation*>> alive(numSCCs);
    /* Resize expired relations sets */
    relationExpirySchedule.resize(numSCCs);

    /* Mark the output relations as alive in the first step */
    for (const AstRelation* relation : translationUnit.getProgram()->getRelations()) {
        if (relation->isComputed()) {
            alive[0].insert(relation);
        }
    }

    /* Compute all alive relations by iterating over all steps in reverse order
       determine the dependencies */
    for (int orderedSCC = 1; orderedSCC < numSCCs; orderedSCC++) {
        /* Add alive set of previous step */
        alive[orderedSCC].insert(alive[orderedSCC - 1].begin(), alive[orderedSCC - 1].end());

        /* Add predecessors of relations computed in this step */
        int scc = topsortSCCGraph->getSCCOrder()[numSCCs - orderedSCC];
        for (const AstRelation* r : topsortSCCGraph->getSCCGraph()->getRelationsForSCC(scc)) {
            for (const AstRelation* predecessor : precedenceGraph->getPredecessors(r)) {
                alive[orderedSCC].insert(predecessor);
            }
        }

        /* Compute expired relations in reverse topological order using the set difference of the alive sets
           between steps. */
        std::set_difference(alive[orderedSCC].begin(), alive[orderedSCC].end(), alive[orderedSCC - 1].begin(),
                alive[orderedSCC - 1].end(), std::inserter(relationExpirySchedule[numSCCs - orderedSCC],
                                                     relationExpirySchedule[numSCCs - orderedSCC].end()));
    }

    return relationExpirySchedule;
}

}  // end of namespace souffle
