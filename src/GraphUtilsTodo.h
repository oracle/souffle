/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file Graph.h
 *
 * A simple utility graph for conducting simple, graph-based operations.
 *
 ***********************************************************************/

#pragma once

#include <deque>
#include <map>
#include <ostream>
#include <stack>
#include <set>
#include <functional>

#include "IndexUtils.h"

namespace souffle {


/*** RDFS ALGORITHM ***/
template <typename Node, typename Compare = std::less<Node>>
class Graph {

protected:

    std::set<Node, Compare> nodes;

    std::map<Node, std::set<Node, Compare>> predecessors;

    std::map<Node, std::set<Node, Compare>> successors;

public:

    const std::set<Node, Compare>& allVertices() const {
        return this->nodes;
    }

    const size_t vertexCount() const {
        return nodes.size();
    }

    virtual void insertVertex(const Node& vertex) {
        if (hasVertex(vertex)) return;
        if (nodes.insert(vertex).second) {
            successor.insert(std::make_pair(node, std::set<Node, Compare>()));
            predecessor.insert(std::make_pair(node, std::set<Node, Compare>()));
        }
    }

    virtual void removeVertex(const Node& vertex) {
        if (!hasVertex(vertex)) return;
        for (const Node& in : predecessors.at(vertex))
            removeEdge(in, vertex);
        for (const Node& out : successors.at(vertex))
            removeEdge(vertex, out);
        nodes.erase(vertex);
        successors.erase(vertex);
        predecessors.erase(vertex);
    }

    const bool hasVertex(const Node& vertex) const {
        return nodes.find(vertex) != nodes.end();
    }

    const size_t vertexCount() const {
        return nodes.size();
    }

    const std::map<Node, std::set<Node, Compare>>& allEdges() const {
        return this->successors;
    }

    virtual void insertEdge(const Node& vertex1, const Node& vertex2) {
        if (hasEdge(vertex1, vertex2)) return;
        if (!hasVertex(vertex1))
            insertVertex(vertex1);
        if (!hasVertex(vertex2))
            insertVertex(vertex2);
        successors.at(vertex1).insert(vertex2);
        predecessors.at(vertex2).insert(vertex1);
    }

    virtual void removeEdge(const Node& vertex1, const Node& vertex2) {
        if (!hasEdge(vertex1, vertex2)) return;
        successors.at(vertex1).erase(vertex2);
        predecessors.at(vertex2).erase(vertex1);
    }

    const bool hasEdge(const Node& vertex1, const Node& vertex2) const {
        return (hasVertex(vertex1) && hasVertex(vertex2))
                       ? successors.at(vertex1).find(vertex2) != successors.at(vertex1).end()
                       : false;
    }

    const bool hasPath(const Node& fromVertex, const Node& toVertex) const {
        if (!hasVertex(fromVertex) || !hasVertex(toVertex)) return false;
        bool found = false;
        bool first = true;
        visitDepthFirst(fromVertex, [&](const Node& current) {
            found = !first && (found || current == toVertex);
            first = false;
        });
        return found;
    }

    const std::set<Node, Compare>& getSuccessors(const Node& vertex) const {
        assert(hasVertex(vertex));
        return successors.at(vertex);
    }

    const void insertSuccessors(const Node& vertex, const std::set<Node, Compare>& vertices) {
        for (const auto& successor : vertices)
            insertEdge(vertex, successor);
    }

    const std::set<Node, Compare>& getPredecessors(const Node& vertex) const {
        assert(hasVertex(vertex));
        return predecessors.at(vertex);
    }

    const void insertPredecessors(const Node& vertex, const std::set<Node, Compare>& vertices) {
        for (const auto& predecessor : vertices)
            insertEdge(predecessor, vertex);
    }

    std::set<Node, Compare> getClique(const Node& fromVertex) const {
        assert(hasVertex(fromVertex));
        std::set<Node, Compare> clique;
        clique.insert(fromVertex);
        for (const auto& toVertex : allVertices())
            if (hasPath(fromVertex, toVertex) && hasPath(toVertex, fromVertex))
                clique.insert(toVertex.first);
        return clique;
    }

    template <typename Lambda>
    void visitDepthFirst(const Node& vertex, const Lambda lambda) const {
        std::set<Node, Compare> visited;
        visitDepthFirst(vertex, lambda, visited);
    }

    friend std::ostream& operator<<(std::ostream& os, const Graph& graph) {
        graph.print(os);
        return os;
    }

    virtual void print(std::ostream& os) const {
        bool first = true;
        out << "digraph {";
        for (const auto& cur : forward) {
            for (const auto& trg : cur.second) {
                if (!first) out << ";";
                out << "\"" << cur.first << "\"" << "->" << "\"" << trg << "\"";
                first = false;
            }
        }
        out << "}" << std::endl;
    }

private:
    template <typename Lambda>
    void visitDepthFirst(const Node& vertex, const Lambda lambda, std::set<Node, Compare>& visited) const {
        lambda(vertex);
        if (!this->hasVertex(vertex)) return;
        for (const auto& it : this->successors.at(vertex))
            if (visited.insert(it).second) this->visitDepthFirst(it, lambda, visited);
    }
};
/***********************/


    const bool hasNeighbours(const Node& vertex) const {
        assert(hasVertex(vertex));
        return (getPredecessors(vertex).size() > 0 || getSuccessors(vertex).size() > 0);
    }

    virtual void mergeVertex(const Node& retainedVertex, const Node& removedVertex) {
        assert(!this->hasNeighbours(retainedVertex) && !this->hasNeighbours(removedVertex));
        removeVertex(removedVertex);
    }

    virtual void contractEdge(const Node& retainedVertex, const Node& removedVertex) {
        assert(this->hasEdge(retainedVertex, removedVertex) || this->hasEdge(removedVertex, retainedVertex));
        insertSuccessors(retainedVertex, getSuccessors(removedVertex));
        insertPredecessors(retainedVertex, getPredecessors(removedVertex));
        removeVertex(removedVertex);
    }

template <typename Node, template <typename...> typename Container, typename Compare = std::less<Node>>
class IndexGraph : public Graph<size_t> {
private:
    index::CollectionIndexTable<Node, Container> indices;

public:
    IndexGraph() : Graph<size_t>() {}

    IndexGraph(const Graph<Node, Compare>& graph) : Graph<size_t>() {
        size_t index = 0;
        for (const Node& vertex : graph.allVertices()) {
            this->insertVertex(index, vertex);
            ++index;
        }
        for (const Node& vertex : graph.allVertices()) {
            index = this->vertexForObject(vertex);
            for (const Node& successor : graph.getSuccessors(vertex)) {
                this->insertEdge(index, this->vertexForObject(successor));
            }
        }
    }

    template<typename OtherNode, template <typename...> typename OtherContainer, typename OtherCompare = std::less<OtherNode>>
    static IndexGraph<Node, Container, Compare> toIndexGraph(IndexGraph<OtherNode, OtherContainer, OtherCompare> oldGraph) {
        IndexGraph<Node, Container, Compare> newGraph = IndexGraph<Node, Container, Compare>();
        for (const size_t vertex : oldGraph.allVertices()) {
            newGraph.insertVertex(vertex, vertex);
        }
        int index;
        for (const size_t vertex : oldGraph.allVertices()) {
            index = newGraph.vertexForObject(vertex);
            for (const size_t successor : oldGraph.getSuccessors(vertex)) {
                newGraph.insertEdge(index, newGraph.vertexForObject(successor));
            }
        }
        return newGraph;
    }

    const bool hasObject(const Node& object) {
        return this->indices.has(object);
    }

    const size_t vertexForObject(const Node& object) const {
        return this->indices.getIndex(object);
    }

    const Container<Node>& objectsForVertex(const size_t vertex) const {
        return this->indices.get(vertex);
    }

    const bool isRecursive(const size_t vertex) const {
        const Container<Node>& objects = objectsForVertex(vertex);
        if (objects.size() == 1) {
            const Node& representative = *objects.begin();
            for (const size_t predecessor : this->getPredecessors(vertex))
                for (const Node& inner : objectsForVertex(predecessor))
                    if (inner == representative)
                        return true;
        }
        return false;
    }

    const bool isRecursive(const Node& object) const {
        return isRecursive(vertexForObject(object));
    }

    virtual void insertVertex(const size_t vertex) = delete;

    void insertVertex(const size_t vertex, const Node& object) {
        Graph<size_t>::insertVertex(vertex);
        this->indices.append(vertex, object);
    }

    template <template <typename...> typename T>
    void insertVertex(const size_t vertex, const T<Node>& objects) {
        Graph<size_t>::insertVertex(vertex);
        this->indices.append(vertex, objects);
    }

    void removeVertex(const size_t vertex) {
        Graph<size_t>::removeVertex(vertex);
        this->indices.remove(vertex);
    }

    void mergeVertex(const size_t retainedVertex, const size_t removedVertex) {
        assert(!this->hasNeighbours(retainedVertex) && !this->hasNeighbours(removedVertex));
        this->indices.moveAppend(removedVertex, retainedVertex);
        Graph<size_t>::mergeVertex(retainedVertex, removedVertex);
    }

    void contractEdge(const size_t retainedVertex, const size_t removedVertex) {
        assert(this->hasEdge(retainedVertex, removedVertex) || this->hasEdge(removedVertex, retainedVertex));
        if (hasEdge(retainedVertex, removedVertex))
            this->indices.moveAppend(removedVertex, retainedVertex);
        else
            this->indices.movePrepend(removedVertex, retainedVertex);
        Graph<size_t>::contractEdge(retainedVertex, removedVertex);
    }

    virtual void print(std::ostream& os) const {
        bool first = true;
        os << "digraph {";
        for (const auto& iter : this->successors) {
            if (!first) os << ";";
            os << iter.first << "[label=\"";
            first = true;
            for (const auto& inner : this->objectsForVertex(iter.first)) {
                if (!first) os << ",";
                os << inner;
                first = false;
            }
            os << "\"]";
            for (const auto& successor : iter.second) {
                os << ";" << iter.first << "->" << successor;
            }
        }
        os << "}" << std::endl;
    }
};

class GraphOrder {
public:

    template <typename Node, typename Compare = std::less<Node>>
    static const std::vector<Node> order(const Graph<Node, Compare>& graph, void(*algorithm)(const Graph<Node, Compare>&, std::function<void(const Node&)>)) {
        std::vector<Node> order;
        algorithm(graph, [&order](const Node& vertex){ order.push_back(vertex); });
        return order;
    }

    template <typename Node, template <typename...> typename Container, typename Compare = std::less<Node>>
    static const std::vector<Node> innerOrder(
            const IndexGraph<Node, Container, Compare>& graph, void(*algorithm)(const Graph<Node, Compare>&, std::function<void(const Node&)>)) {
        std::vector<size_t> outerOrder;
        algorithm(graph, [&outerOrder](const size_t& vertex){ outerOrder.push_back(vertex); });
        std::vector<Node> innerOrder;
        for (const size_t index : outerOrder) {
            const Container<Node>& objectsForVertex = graph.objectsForVertex(index);
            innerOrder.insert(innerOrder.end(), objectsForVertex.begin(), objectsForVertex.end());
        }
        return innerOrder;
    }

    template <typename Node, template <typename...> typename Container, typename Compare = std::less<Node>>
    static const std::vector<size_t> outerOrder(
            IndexGraph<Node, Container, Compare>& graph, void(*algorithm)(const Graph<Node, Compare>&, std::function<void(const Node&)>)) {
        std::vector<size_t> order;
        algorithm(graph, [&order](const size_t& vertex){ order.push_back(vertex); });
        return order;
    }
};

class GraphTransform {

private:

   /*
    * Compute strongly connected components using Gabow's algorithm (cf. Algorithms in
    * Java by Robert Sedgewick / Part 5 / Graph *  algorithms). The algorithm has linear
    * runtime.
    */
    template <typename Node, typename Compare = std::less<Node>>
    static void toSCCGraph(const Graph<Node, Compare>& graph, IndexGraph<Node, std::set, Compare>& sccGraph, const Node& w, std::map<Node, int>& preOrder, size_t& counter, std::stack<Node>& S, std::stack<Node>& P) {

        preOrder[w] = counter++;

        S.push(w);
        P.push(w);

        for(const Node& t : graph.getPredecessors(w))
            if (preOrder[t] == -1)
                toSCCGraph(graph, sccGraph, t, preOrder, counter, S, P);
            else if (!sccGraph.hasObject(t))
                while (preOrder[P.top()] > preOrder[t])
                    P.pop();

        if (P.top() == w)
            P.pop();
        else
            return;

        Node v;
        do {
            v = S.top();
            S.pop();
            sccGraph.insertVertex(sccGraph.vertexCount(), v);
        } while(v != w);

    }


public:

    template <typename Node, typename Compare = std::less<Node>>
    static IndexGraph<Node, std::set, Compare> toSCCGraph(const Graph<Node, Compare> graph) {
            size_t counter = 0;
            std::stack<Node> S, P;
            std::map<Node, int> preOrder;
            IndexGraph<Node, std::set, Compare> sccGraph = IndexGraph<Node, std::set, Compare>();
            for (const Node& vertex : graph.allVertices())
                   preOrder[vertex] = -1;
            for (const Node& vertex : graph.allVertices())
                if (preOrder[vertex]  == -1)
                    toSCCGraph(graph, sccGraph, vertex, preOrder, counter, S, P);
            for (const Node& vertex : graph.allVertices())
                for (const Node& successor : graph.getSuccessors(vertex))
                    if (vertex != successor)
                        sccGraph.insertEdge(sccGraph.vertexForObject(vertex), sccGraph.vertexForObject(successor));
            return sccGraph;
    }

};

class GraphSearch {

private:

    template <typename Lambda, typename Node, typename Compare>
    static void depthFirst(const Graph<Node, Compare>& graph, const Node& vertex, const Lambda lambda, std::set<Node, Compare>& visited) {
        lambda(vertex);
        for (const auto& it : graph.getSuccessors(vertex))
            if (visited.insert(it).second)
                depthFirst(graph, it, lambda, visited);
    }


    template <typename Lambda, typename Node, typename Compare>
    static void khansAlgorithm(const Graph<Node, Compare>& graph, const Node& vertex, const Lambda lambda, std::set<Node, Compare>& visited) {
        auto it = graph.getSuccessors(vertex).begin();
        for (; it != graph.getSuccessors(vertex).end(); ++it) {
            if (visited.find(*it) == visited.end()) {
                bool hasUnvisitedPredecessor = false;
                for (const Node& predecessor : graph.getPredecessors(*it)) {
                    if (visited.find(predecessor) == visited.end()) {
                        hasUnvisitedPredecessor = true;
                        break;
                    }
                }
                if (!hasUnvisitedPredecessor) {
                    lambda(*it);
                    visited.insert(*it);
                    khansAlgorithm(graph, vertex, lambda, visited);
                }
            }
        }

        if (visited.find(*it) == visited.end()) return;

        bool hasUnvisitedPredecessor = false;
        for (const Node& predecessor : graph.getPredecessors(*it)) {
            if (visited.find(predecessor) == visited.end()) {
                hasUnvisitedPredecessor = true;
                break;
            }
        }

        bool hasUnvisitedSuccessor = false;
        for (const Node& successor : graph.getSuccessors(*it)) {
            if (visited.find(successor) == visited.end()) {
                hasUnvisitedSuccessor = true;
                break;
            }
        }

        if (!hasUnvisitedPredecessor && hasUnvisitedSuccessor) {
            khansAlgorithm(graph, *it, lambda, visited);
        }

    }

public:

    template <typename Lambda, typename Node, typename Compare>
    static void depthFirst(const Graph<Node, Compare>& graph, const Lambda lambda) {
        std::set<Node, Compare> visited = std::set<Node, Compare>();
        for (const Node& vertex : graph.allVertices()) {
            if (graph.getPredecessors(vertex).empty()) {
                lambda(vertex);
                visited.insert(vertex);
                if (!graph.getSuccessors(vertex).empty()) {
                    depthFirst(graph, vertex, lambda, visited);
                }
            }
        }
    }

    template <typename Lambda, typename Node, typename Compare = std::less<Node>>
    static void khansAlgorithm(const Graph<Node, Compare>& graph, Lambda lambda) {
        std::set<Node, Compare> visited = std::set<Node, Compare>();
        for (const Node& vertex : graph.allVertices()) {
            if (graph.getPredecessors(vertex).empty()) {
                lambda(vertex);
                visited.insert(vertex);
                if (!graph.getSuccessors(vertex).empty()) {
                    khansAlgorithm(graph, vertex, lambda, visited);
                }
            }
        }
    }

    // TODO
    template <typename Lambda, typename Node, typename Compare = std::less<Node>>
    static void reverseDepthFirst(const Graph<Node, Compare>& graph, Lambda lambda) {}
};

}  // end of namespace souffle

