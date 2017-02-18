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

#include <map>
#include <ostream>
#include <set>
#include <stack>

#include "IndexUtils.h"

namespace souffle {

template <typename Node, typename Compare = std::less<Node>>
class Graph {

protected:

    std::set<Node, Compare> nodes;

    std::map<Node, std::set<Node, Compare>> predecessors;

    std::map<Node, std::set<Node, Compare>> successors;

public:

    const std::set<Node, Compare>& allVertices() const {
        return nodes;
    }

    const size_t vertexCount() const {
        return nodes.size();
    }

    virtual void insertVertex(const Node& vertex) {
        if (hasVertex(vertex)) return;
        if (nodes.insert(vertex).second) {
            successors.insert(std::make_pair(vertex, std::set<Node, Compare>()));
            predecessors.insert(std::make_pair(vertex, std::set<Node, Compare>()));
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

    const std::map<Node, std::set<Node, Compare>>& allEdges() const {
        return successors;
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
                clique.insert(toVertex);
        return clique;
    }

    virtual void joinVertices(const Node& subjectVertex, const Node& deletedVertex) {
        insertSuccessors(subjectVertex, getSuccessors(deletedVertex));
        insertPredecessors(subjectVertex, getPredecessors(deletedVertex));
        removeVertex(deletedVertex);
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
        os << "digraph {";
        for (const auto& vertex : successors) {
            for (const auto& successor : vertex.second) {
                if (!first) os << ";";
                os << "\"" << vertex.first << "\"" << "->" << "\"" << successor << "\"";
                first = false;
            }
        }
        os << "}" << std::endl;
    }

private:

    template <typename Lambda>
    void visitDepthFirst(const Node& vertex, const Lambda lambda, std::set<Node, Compare>& visited) const {
        lambda(vertex);
        if (!hasVertex(vertex)) return;
        for (const auto& it : successors.at(vertex))
            if (visited.insert(it).second) visitDepthFirst(it, lambda, visited);
    }
};

template <template <typename> class Table, typename Node>
class HyperGraph : public Graph<size_t> {

    private:

        Table<Node> table;

    public:

        template <typename Compare = std::less<Node>>
        static HyperGraph<Table, Node> toHyperGraph(Graph<Node, Compare> oldGraph) {
            HyperGraph<Table, Node> newGraph = HyperGraph<Table, Node>();
            size_t index = 0;
            for (const size_t vertex : oldGraph.allVertices()) {
                newGraph.insertVertex(index, vertex);
                index++;
            }
            for (const size_t vertex : oldGraph.allVertices()) {
                index = newGraph.vertexTable.getIndex(vertex);
                for (const size_t successor : oldGraph.getSuccessors(vertex)) {
                    newGraph.insertEdge(index, newGraph.vertexTable().getIndex(successor));
                }
            }
            return newGraph;
        }

        template<template <typename> class OtherTable, typename OtherNode>
        static HyperGraph<Table, Node> toHyperGraph(HyperGraph<OtherTable, OtherNode> oldGraph) {
            HyperGraph<Table, Node> newGraph = HyperGraph<Table, Node>();
            size_t index = 0;
            for (const size_t vertex : oldGraph.allVertices()) {
                newGraph.insertVertex(vertex, vertex);
            }
            for (const size_t vertex : oldGraph.allVertices()) {
                index = newGraph.vertexTable.getIndex(vertex);
                for (const size_t successor : oldGraph.getSuccessors(vertex)) {
                    newGraph.insertEdge(index, newGraph.vertexTable().getIndex(successor));
                }
            }
            return newGraph;
        }

        const Table<Node>& vertexTable() const {
            return this->table;
        }

        void insertVertex(const size_t vertex) {
            Graph<size_t>::insertVertex(vertex);
            this->table.set(vertex);
        }

        void insertVertex(const size_t vertex, const Node& object) {
            Graph<size_t>::insertVertex(vertex);
            this->table.setIndex(object, vertex);
        }

        template <template <typename...> class T>
        void insertVertex(const size_t vertex, const T<Node>& objects) {
            Graph<size_t>::insertVertex(vertex);
            this->table.set(vertex, objects);
        }

        void removeVertex(const size_t vertex) {
            Graph<size_t>::removeVertex(vertex);
            this->table.remove(vertex);
        }

        void appendToVertex(const size_t vertex, const Node& object) {
            if (!hasVertex(vertex)) Graph<size_t>::insertVertex(vertex);
            this->table.append(vertex, object);
        }

        template <template <typename...> class T>
        void appendToVertex(const size_t vertex, const T<Node>& objects) {
            if (!hasVertex(vertex)) Graph<size_t>::insertVertex(vertex);
            this->table.append(vertex, objects);
        }

        void prependToVertex(const size_t vertex, const Node& object) {
            if (!hasVertex(vertex)) Graph<size_t>::insertVertex(vertex);
            this->table.prepend(vertex, object);
        }

        template <template <typename...> class T>
        void prependToVertex(const size_t vertex, const T<Node>& objects) {
            if (!hasVertex(vertex)) Graph<size_t>::insertVertex(vertex);
            this->table.prepend(vertex, objects);
        }

        void joinVertices(const size_t subjectVertex, const size_t deletedVertex) {
            if (hasEdge(deletedVertex, subjectVertex))
                this->table.moveAppend(deletedVertex, subjectVertex);
            else
                this->table.movePrepend(deletedVertex, subjectVertex);
            Graph<size_t>::joinVertices(subjectVertex, deletedVertex);
        }


        virtual void print(std::ostream& os) const {
            bool first = true;
            os << "digraph {";
            for (const auto& iter : this->successors) {
                if (!first) os << ";\n";
                os << "\"" << iter.first << "\"" << " [label=\"";
                first = true;
                for (const auto& inner : this->table.get(iter.first)) {
                    if (!first) os << ",";
                    os << inner;
                    first = false;
                }
                os << "\"]";
                for (const auto& successor : iter.second) {
                    os << ";\n" << "\"" << iter.first << "\"" << " -> " << "\"" << successor << "\"";
                }
            }
            os << "}\n" << std::endl;
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

    template <template <typename> typename Table, typename Node, typename Compare = std::less<Node>>
    static const std::vector<Node> innerOrder(const HyperGraph<Table, Node>& graph, void(*algorithm)(const Graph<Node, Compare>&, std::function<void(const Node&)>)) {
        std::vector<size_t> outerOrder;
        algorithm(graph, [&outerOrder](const size_t& vertex){ outerOrder.push_back(vertex); });
        std::vector<Node> innerOrder;
        for (const size_t index : outerOrder) {
            const auto& objectsForVertex = graph.vertexTable().get(index);
            innerOrder.insert(innerOrder.end(), objectsForVertex.begin(), objectsForVertex.end());
        }
        return innerOrder;
    }

    template <template <typename> typename Table, typename Node, typename Compare = std::less<Node>>
    static const std::vector<size_t> outerOrder(
            HyperGraph<Table, Node>& graph, void(*algorithm)(const Graph<Node, Compare>&, std::function<void(const Node&)>)) {
        std::vector<size_t> order;
        algorithm(graph, [&order](const size_t& vertex){ order.push_back(vertex); });
        return order;
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
