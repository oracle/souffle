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
 * @file RamRecords.cpp
 *
 * Utilities for handling records in the interpreter
 *
 ***********************************************************************/

#include <iostream>

#include <limits>
#include <map>
#include <vector>

#include "RamRecords.h"
#include "Util.h"

namespace souffle {

namespace {

    using namespace std;

    /**
     * A bidirectional mapping between tuples and reference indices.
     */
    class RecordMap {

        /** The arity of the stored tuples */
        int arity;

        /** The mapping from tuples to references/indices */
        map<vector<RamDomain>,RamDomain> r2i;

        /** The mapping from indices to tuples */
        vector<vector<RamDomain>> i2r;

    public:

        RecordMap(int arity)
            : arity(arity), i2r(1) {}       // note: index 0 element left free

        /**
         * Packs the given tuple -- and may create a new reference if necessary.
         */
        RamDomain pack(const RamDomain* tuple) {
            vector<RamDomain> tmp(arity);
            for(int i=0; i<arity; i++) {
                tmp[i] = tuple[i];
            }

            RamDomain index;
            #pragma omp critical (record_pack)
            {
                auto pos = r2i.find(tmp);
                if (pos != r2i.end()) {
                    index = pos->second;
                } else {

                    #pragma omp critical (record_unpack)
                    {
                        i2r.push_back(tmp);
                        index = i2r.size() - 1;
                        r2i[tmp] = index;

                        // assert that new index is smaller than the range
                        assert(index != std::numeric_limits<RamDomain>::max());
                    }
                }
            }

            return index;
        }

        /**
         * Obtains a pointer to the tuple addressed by the given index.
         */
        RamDomain* unpack(RamDomain index) {

            RamDomain* res;

            #pragma omp critical (record_unpack)
            res = &(i2r[index][0]);

            return res;
        }

    };


    /**
     * The static access function for record maps of certain arities.
     */
    RecordMap& getForArity(int arity) {

        // the static container -- filled on demand
        static map<int,RecordMap> maps;

        // get container if present
        auto pos = maps.find(arity);
        if (pos != maps.end()) {
            return pos->second;
        }

        // create new container if required
        maps.emplace(arity, arity);
        return getForArity(arity);
    }

}

RamDomain pack(RamDomain* tuple, int arity) {
    // conduct the packing
    return getForArity(arity).pack(tuple);
}

RamDomain* unpack(RamDomain ref, int arity) {
    // conduct the unpacking
    return getForArity(arity).unpack(ref);
}

RamDomain getNull() {
    return 0;
}

bool isNull(RamDomain ref) {
    return ref == 0;
}

} // end of namespace souffle

