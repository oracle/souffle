/*
 * Copyright (c) 2013-14, Oracle and/or its affiliates.
 *
 * All rights reserved.
 */

/************************************************************************
 *
 * @file MaxMatching.cpp
 *
 * Implements classes for the max matching calculation
 ***********************************************************************/

#include "RamMaxMatching.h"

/**
* Class MaxMatching
*/

/** implementation of bredth first search */
bool RamMaxMatching::bfSearch() {
    SearchColumns u;
    std::queue<SearchColumns> bfQueue;

    // Build layers
    for (Graph::iterator it = graph.begin(); it != graph.end(); ++it) {
        if (getMatch(it->first) == NIL) {
            distance[it->first] = 0;
            bfQueue.push(it->first);
        }
        else { 
            distance[it->first] = INF;
        }
    }

    distance[NIL] = INF;

    while (!bfQueue.empty()) {
        u = bfQueue.front(); 
        bfQueue.pop();

        ASSERT(u != NIL); 

        const Edges& children = graph[u];
        for (Edges::iterator it = children.begin(); it != children.end(); ++it) {
            SearchColumns mv = getMatch(*it);
            if (getDistance(mv) == INF) {
                distance[mv] = getDistance(u) + 1;
                if (mv != NIL)
                  bfQueue.push(mv);
            }
        }
    }

    return (getDistance(0) != INF);
}

/** implementation of depth first search */
bool RamMaxMatching::dfSearch(SearchColumns u) {
    if (u != 0) {
        Edges& children = graph[u];
        for (Edges::iterator it = children.begin(); it != children.end(); ++it) {
            SearchColumns v = *it;
            if (getDistance(getMatch(v)) == getDistance(u)+1) {
                if (dfSearch(getMatch(v))) {
                    match[u] = v;
                    match[v] = u;
                    return true;
                }
            }
        }

        distance[u] = INF;
        return false;
    }
    return true;
}

