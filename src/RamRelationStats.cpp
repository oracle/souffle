/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
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
    for (auto it = rel.begin(); it != rel.end() && count < sample_size; ++it, ++count) {
        const RamDomain* tuple = *it;

        // compute cardinality of columns
        for (std::size_t i = 0; i < rel.getArity(); ++i) {
            columns[i].insert(tuple[i]);
        }
    }

    // create the resulting statistics object
    RamRelationStats stats;

    stats.arity = rel.getArity();
    stats.size = rel.size();
    stats.sample_size = count;

    for (std::size_t i = 0; i < rel.getArity(); i++) {
        // estimate the cardinality of the columns
        uint64_t card = 0;
        if (count > 0) {
            // based on the observed probability
            uint64_t cur = columns[i].size();
            double p = ((double)cur / (double)count);

            // obtain an estimate of the overall cardinality
            card = (uint64_t)(p * rel.size());

            // make sure that it is at least what you have seen
            if (card < cur) {
                card = cur;
            }
        }

        // add result
        stats.cardinalities.push_back(card);
    }

    // done
    return stats;
}

}  // end of namespace souffle
