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

/***************************************************************************
 *
 * @file RamAutoIndex.h
 *
 * Defines classes to automatically compute the minimal indexes for a table
 *
 ***************************************************************************/

#pragma once

#include <stdint.h>
#include <string.h>

#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <iostream>
#include <limits>
#include <math.h>

#include "RamTypes.h"
#include "RamMaxMatching.h"

namespace souffle {

class RamAutoIndex {

public:
    typedef std::vector<int>  LexicographicalOrder;
    typedef std::vector< LexicographicalOrder > OrderCollection;

protected:
    typedef std::set<SearchColumns> Chain;
    typedef std::vector<Chain> ChainOrderMap;
    typedef std::set<SearchColumns> SearchSet;

    SearchSet searches;          // set of search patterns on table 
    OrderCollection orders;      // collection of lexicographical orders 
    ChainOrderMap chainToOrder;  // maps order index to set of searches covered by chain 

    RamMaxMatching matching;     // matching problem for finding minimal number of orders

public:
    RamAutoIndex() {}
    /** add new key to an Index Set */
    inline void addSearch(SearchColumns cols) {
        if (cols != 0) searches.insert(cols);
    }

    /** obtains access to the internally stored keys **/
    const SearchSet& getSearches() const {
        return searches;
    }

    const LexicographicalOrder getLexOrder(SearchColumns cols) const { 
       int idx = map(cols); 
       return orders[idx]; 
    }
    
    const OrderCollection getAllOrders() const { 
       return orders; 
    }

    /** check whether number of bits in k is not equal
        to number of columns in lexicographical order */
    bool isSubset(SearchColumns cols) const {
       int idx = map(cols);
       return card(cols) < orders[idx].size();
    }

    /** map the keys in the key set to lexicographical order */
    void solve();

   /** convert from a representation of A verticies to B verticies */
    static SearchColumns toB(SearchColumns a) {
        SearchColumns msb = 1;
        msb <<= (4*8-1);
        return (a | msb);
    }

    /** convert from a representation of B verticies to A verticies */
    static SearchColumns toA(SearchColumns b) {
        SearchColumns msb = 1;
        msb <<= (4*8-1);
        return (b xor msb);
    }

protected:
    /** count the number of bits in key */
    // TODO: replace by intrinsic of GCC 
    static size_t card(SearchColumns cols) {
       size_t sz = 0, idx=1; 
       for (size_t i=0; i < sizeof(SearchColumns) * 8; i++) { 
           if(idx & cols) {
               sz ++; 
           }
           idx *= 2; 
       } 
       return sz; 
    }

    /** maps search columns to an lexicographical order (labeled by a number) */
    int map(SearchColumns cols) const {
       ASSERT(orders.size() == chainToOrder.size() && "Order and Chain Sizes do not match!!");
       int i=0; 
       for(auto it = chainToOrder.begin(); it != chainToOrder.end(); ++it, ++i) {
           if (it->find(cols) != it->end()) {
               ASSERT((size_t)i < orders.size());
               return i;
           }
       }
       abort();  
    }


    /** determine if key a is a strict subset of key b*/
    static bool isStrictSubset(SearchColumns a, SearchColumns b){
        SearchColumns tt = static_cast<SearchColumns>(std::numeric_limits<SearchColumns>::max());
        return (~(a) | (b)) == tt && a != b;
    }

    /** insert an index based on the delta*/
    void insertIndex(std::vector<int>& ids, SearchColumns delta) {
        int pos = 0;
        SearchColumns mask = 0;

        while(mask < delta) {
            mask = SearchColumns(1<<(pos));
            SearchColumns result = (delta) & (mask);
            if (result) {
                ids.push_back(pos);
            }
            pos++;
        }
    }

    /** given an unmapped node from set A we follow it from set B until it cannot be matched from B
        if not mateched from B then umn is a chain*/
    Chain getChain(const SearchColumns umn, const RamMaxMatching::Matchings& match);

    /** get all chains from the matching */
    const ChainOrderMap getChainsFromMatching(const RamMaxMatching::Matchings& match, const SearchSet& nodes);

    /** get all nodes which are unmated from A-> B */
    const SearchSet getUnmatchedKeys(const RamMaxMatching::Matchings& match, const SearchSet& nodes) {
        ASSERT(nodes.size() > 0);
        SearchSet unmatched;

        // For all nodes n such that n is not in match
        for(SearchSet::const_iterator it = nodes.begin(); it != nodes.end(); ++it) {
            if(match.find(*it) == match.end()){
              unmatched.insert(*it);
            }
        }
        return unmatched;
    }
};

} // end of namespace souffle

