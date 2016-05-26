/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file CompiledRamRecord.h
 *
 * The central interface for handling records in the compiled execution.
 *
 ***********************************************************************/

#pragma once

#include <limits>
#include <vector>
#include <unordered_map>

#include "CompiledRamTuple.h"
#include "ParallelUtils.h"

namespace souffle {

// ----------------------------------------------------------------------------
//                              Declarations
// ----------------------------------------------------------------------------

/**
 * A function packing a tuple of the given arity into a reference.
 */
template<typename Tuple>
RamDomain pack(const Tuple& tuple);

/**
 * A function obtaining a pointer to the tuple addressed by the given reference.
 */
template<typename Tuple>
const Tuple& unpack(RamDomain ref);

/**
 * Obtains the null-reference constant.
 */
template<typename TupleType>
RamDomain getNull();

/**
 * Determines whether the given reference is the null reference encoding
 * the absence of any nested record.
 */
template<typename TupleType>
bool isNull(RamDomain ref);



// ----------------------------------------------------------------------------
//                              Definitions
// ----------------------------------------------------------------------------


template<typename TupleType>
RamDomain getNull() {
    return 0;
}

template<typename TupleType>
bool isNull(RamDomain ref) {
    return ref == 0;
}

namespace detail {

    /**
     * A bidirectional mapping between tuples and reference indices.
     */
    template<typename Tuple>
    class RecordMap {

        /** The definition of the tuple type handled by this instance */
        typedef Tuple tuple_type;

        /** The mapping from tuples to references/indices */
        std::unordered_map<tuple_type,RamDomain> r2i;

        /** The mapping from indices to tuples */
        std::vector<tuple_type> i2r;

        /** a lock for the pack operation */
        Lock pack_lock;

        /** a lock for the unpack operation */
        Lock unpack_lock;

    public:

        RecordMap() : i2r(1) {}       // note: index 0 element left free

        /**
         * Packs the given tuple -- and may create a new reference if necessary.
         */
        RamDomain pack(const tuple_type& tuple) {

            RamDomain index;

            {
                // lock pack operation
                auto leas = pack_lock.acquire();     // lock hold till end of scope
                (void) leas; // avoid warning

                // try lookup
                auto pos = r2i.find(tuple);
                if (pos != r2i.end()) {

                    // take the previously assigned value
                    index = pos->second;

                } else {

                    {
                        // lock unpack operation
                        auto leas = unpack_lock.acquire();
                        (void) leas; // avoid warning

                        // create new entry
                        i2r.push_back(tuple);
                        index = i2r.size() - 1;
                        r2i[tuple] = index;

                        // assert that new index is smaller than the range
                        assert(index != std::numeric_limits<RamDomain>::max());
                    }
                }
            }

            // done
            return index;
        }

        /**
         * Obtains a pointer to the tuple addressed by the given index.
         */
        const tuple_type& unpack(RamDomain index) {

            tuple_type* res;

            {
                auto leas = unpack_lock.acquire();
                (void) leas; // avoid warning
                res = &(i2r[index]);
            }

            return *res;
        }

    };


    /**
     * The static access function for record of a certain type.
     */
    template<typename Tuple>
    RecordMap<Tuple>& getRecordMap() {
        static RecordMap<Tuple> map;
        return map;
    }

}


template<typename Tuple>
RamDomain pack(const Tuple& tuple) {
    return detail::getRecordMap<Tuple>().pack(tuple);
}

template<typename Tuple>
const Tuple& unpack(RamDomain ref) {
    return detail::getRecordMap<Tuple>().unpack(ref);
}

} // end of namespace souffle

