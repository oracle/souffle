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
 * @file Graph.h
 *
 * A simple utility graph for conducting simple, graph-based operations.
 *
 ***********************************************************************/

#pragma once

#include <map>

/**
 * A simple graph structure for graph-based operations.
 */
template<typename Node>
class Graph {

    // not a very efficient but simple graph representation
    std::set<Node> nodes;                       // all the nodes in the graph
    std::map<Node,std::set<Node>> forward;      // all edges forward directed
    std::map<Node,std::set<Node>> backward;     // all edges backward

public:

    /**
     * Adds a new edge from the given node to the target node.
     */
    void addEdge(const Node& from, const Node& to) {
        addNode(from);
        addNode(to);
        forward[from].insert(to);
        backward[to].insert(from);
    }

    /**
     * Adds a node.
     */
    void addNode(const Node& node) {
        auto iter = nodes.insert(node);
        if (iter.second) {
            forward.insert(std::make_pair(node, std::set<Node>()));
            backward.insert(std::make_pair(node, std::set<Node>()));
        }
    }

    /** Obtains a reference to the set of all nodes */
    const std::set<Node>& getNodes() const {
        return nodes;
    }

    /** Returns the set of nodes the given node has edges to */
    const std::set<Node>& getEdges(const Node& from) {
        assert(contains(from));
        return forward.find(from)->second;
    }

    /** Determines whether the given node is present */
    bool contains(const Node& node) const {
        return nodes.find(node) != nodes.end();
    }

    /** Determines whether the given edge is present */
    bool contains(const Node& from, const Node& to) const {
        auto pos = forward.find(from);
        if (pos == forward.end()) return false;
        auto p2 = pos->second.find(to);
        return p2 != pos->second.end();
    }

    /** Determines whether there is a directed path between the two nodes */
    bool reaches(const Node& from, const Node& to) const {
        // quick check
        if (!contains(from) || !contains(to)) return false;

        // conduct a depth-first search starting at from
        bool found = false;
        bool first = true;
        visitDepthFirst(from, [&](const Node& cur) {
            found = !first && (found || cur == to);
            first = false;
        });
        return found;
    }

    /** Obtains the set of all nodes in the same clique than the given node */
    std::set<Node> getClique(const Node& node) const {
        std::set<Node> res;
        res.insert(node);
        for(const auto& cur : getNodes()) {
            if (reaches(node,cur) && reaches(cur,node)) res.insert(cur);
        }
        return res;
    }

    /** A generic utility for depth-first visits */
    template<typename Lambda>
    void visitDepthFirst(const Node& node, const Lambda& lambda) const {
        std::set<Node> visited;
        visitDepthFirst(node, lambda, visited);
    }

    /** Enables graphs to be printed (e.g. for debugging) */
    void print(std::ostream& out) const {
        bool first = true;
        out << "{";
        for(const auto& cur : forward) {
            for(const auto& trg : cur.second) {
                if (!first) out << ",";
                out << cur.first << "->" << trg;
                first = false;
            }
        }
        out << "}";
    }

    friend std::ostream& operator<<(std::ostream& out, const Graph& g) {
        g.print(out);
        return out;
    }

private:

    /** The internal implementation of depth-first visits */
    template<typename Lambda>
    void visitDepthFirst(const Node& node, const Lambda& lambda, std::set<Node>& visited) const {
        lambda(node);
        auto pos = forward.find(node);
        if (pos == forward.end()) return;
        for(const auto& cur : pos->second) {
            if (visited.insert(cur).second) {
                visitDepthFirst(cur, lambda, visited);
            }
        }
    }

};
