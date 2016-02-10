/*
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All Rights reserved
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
 * @file MaxMatching.h
 *
 * Defines classes of the Hopcroft-Karp algorithm
 * Source: http://en.wikipedia.org/wiki/Hopcroft%E2%80%93Karp_algorithm#Pseudocode
 ***********************************************************************/

#pragma once

#include <cstring>
#include <set>
#include <map>
#include <queue>
#include <functional>
#include <iostream>

#include "Util.h"
#include "RamTypes.h"

#define NIL 0
#define INF -1

// define if enable unit tests
#define M_UNIT_TEST 

class RamMaxMatching {

public:
    typedef std::map<SearchColumns, SearchColumns, std::greater<SearchColumns> > Matchings;
    typedef std::set<SearchColumns, std::greater<SearchColumns> > Nodes;

private:
    typedef std::set<SearchColumns> Edges;
    typedef std::map<SearchColumns, Edges > Graph;
    typedef std::map<SearchColumns, int> Distance;

private:
    Matchings match; // if x not in match assume match[x] == 0 else a value. Needs both edges...
    Graph graph; // Only traversed not modified
    Distance distance; // if x not in distance assume distance[x] == inf else a distance, distance [0] special

public:

    /** calculates max matching */
    const Matchings& calculate() {
        while (bfSearch()) {
            for (Graph::iterator it = graph.begin(); it != graph.end(); ++it) {
                if (getMatch(it->first) == NIL) {
                    dfSearch(it->first);
                }
            }
        }
        return match;
    }

    int getNumMatchings(){
       return match.size()/2;
    }

    /** add an edge to the bi-partite graph */
    void addEdge(SearchColumns u, SearchColumns v) {
        if (graph.find(u) == graph.end()) {
            Edges vals;
            vals.insert(v);
            graph.insert(make_pair(u, vals));
        }
        else {
          graph[u].insert(v);
        }
    }

protected:
    /** returns match of v */
    inline SearchColumns getMatch(SearchColumns v) {
        Matchings::iterator it = match.find(v);
        if (it == match.end()) return NIL; else return it->second;
    }

    /** returns distance of v */
    inline int getDistance(int v) {
        Distance::iterator it = distance.find(v);
        if (it == distance.end()) return INF; else return it->second;
    }

    /** breadth first search */
    bool bfSearch();

    /** depth first search */
    bool dfSearch(SearchColumns u);

};
