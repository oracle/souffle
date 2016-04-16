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
 * @file RamTable.h
 *
 * Defines classes Table, TupleBlock, Index, and HashBlock for implementing
 * an ram relational database. A table consists of a linked list of
 * tuple blocks that contain tuples of the table. An index is a hash-index
 * whose hash table is stored in Index. The entry of a hash table entry
 * refer to HashBlocks that are blocks of pointers that point to tuples
 * in tuple blocks with the same hash.
 *
 ***********************************************************************/

#include "RamRelationStats.h"

#include "BTree.h"

namespace souffle {

RamRelationStats RamRelationStats::extractFrom(const RamRelation& rel, uint32_t sample_size) {

    // write each column in its own set
    std::vector<btree_set<RamDomain>> columns(rel.getArity());

    // analyze sample
    uint32_t count = 0;
    for(auto it = rel.begin(); it != rel.end() && count < sample_size; ++it, ++count) {
        const RamDomain* tuple = *it;

        // compute cardinality of columns
        for(std::size_t i =0; i < rel.getArity() ; ++i) {
            columns[i].insert(tuple[i]);
        }
    }


    // create the resulting statistics object
    RamRelationStats stats;

    stats.arity = rel.getArity();
    stats.size = rel.size();
    stats.sample_size = count;

    for(std::size_t i=0; i<rel.getArity(); i++) {

        // estimate the cardinality of the columns
        uint64_t card = 0;
        if (count > 0) {

            // based on the observed probability
            uint64_t cur = columns[i].size();
            double p = ( (double)cur / (double)count );

            // obtain an estimate of the overall cardinality
            card = (uint64_t)(p * rel.size());

            // make sure that it is at least what you have seen
            if (card < cur) card = cur;
        }

        // add result
        stats.cardinalities.push_back(card);
    }

    // done
    return stats;

}

} // end of namespace souffle

