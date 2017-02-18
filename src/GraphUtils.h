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

}  // end of namespace souffle
