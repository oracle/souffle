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
 * @file RamRelationStats.h
 *
 * A set of utilities for extracting and handling statistical data on
 * the data stored within relations.
 *
 ***********************************************************************/

#pragma once

#include <vector>
#include <limits>

#include "RamRelation.h"

namespace souffle {

/** The type to reference indices */
typedef unsigned Column;

/**
 * A summary of statistical properties of a ram relation.
 */
class RamRelationStats {

    /** The arity - accurate */
    uint8_t arity;

    /** The number of tuples - accurate */
    uint64_t size;

    /** The sample size estimations are based on */
    uint32_t sample_size;

    /** The cardinality of the various components of the tuples - estimated */
    std::vector<uint64_t > cardinalities;

public:

    RamRelationStats() : arity(0), size(0), sample_size(0) {}

    RamRelationStats(uint64_t size, const std::vector<uint64_t>& cards)
        : arity(cards.size()), size(size), sample_size(0), cardinalities(cards) {}

    RamRelationStats(const RamRelationStats&) = default;
    RamRelationStats(RamRelationStats&&) = default;

    RamRelationStats& operator=(const RamRelationStats&) = default;
    RamRelationStats& operator=(RamRelationStats&&) = default;

    /**
     * A factory function extracting statistical information form the given relation
     * base on a given sample size. If the sample size is not specified, the full
     * relation will be processed.
     */
    static RamRelationStats extractFrom(const RamRelation& rel, uint32_t sample_size = std::numeric_limits<uint32_t>::max());

    uint8_t getArity() const {
        return arity;
    }

    uint64_t getCardinality() const {
        return size;
    }

    uint32_t getSampleSize() const {
        return sample_size;
    }

    uint64_t getEstimatedCardinality(Column c) const {
        if (c >= cardinalities.size()) return 0;
        return cardinalities[c];
    }

    void print(std::ostream& out) const {
        out << cardinalities;
    }

    friend std::ostream& operator<<(std::ostream& out, const RamRelationStats& stats) {
        stats.print(out);
        return out;
    }

};

} // end of namespace souffle

