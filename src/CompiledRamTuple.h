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
 * @file CompiledRamRelation.h
 *
 * The central file covering the data structure utilized by
 * the souffle compiler for representing relations in compiled queries.
 *
 ***********************************************************************/

#pragma once

#include <iostream>

#include "RamTypes.h"

namespace souffle {

namespace ram {

	/**
	 * The type of object stored within relations representing the actual
	 * tuple value. Each tuple consists of a constant number of components.
	 *
	 * @tparam Domain the domain of the component values
	 * @tparam arity the number of components within an instance
	 */
    template<typename Domain, std::size_t _arity>
    struct Tuple {

        // some features for template meta programming
        typedef Domain value_type;
        enum { arity = _arity };

        // the stored data
        Domain data[arity];

        // constructores, destructors and assignment are default

        // provide access to components
        const Domain& operator[](std::size_t index) const {
            return data[index];
        }

        // provide access to components
        Domain& operator[](std::size_t index) {
            return data[index];
        }

        // a comparison operation
        bool operator==(const Tuple& other) const {
            for(std::size_t i = 0; i<arity; i++) {
                if (data[i] != other.data[i]) return false;
            }
            return true;
        }

        // inequality comparison
        bool operator!=(const Tuple& other) const {
            return !(*this == other);
        }

        // required to put tuples into e.g. a std::set container
        bool operator<(const Tuple& other) const {
            for(std::size_t i = 0; i < arity; ++i) {
                if (data[i] < other.data[i]) return true;
                if (data[i] > other.data[i]) return false;
            }
            return false;
        }

        // required to put tuples into e.g. a btree container
        bool operator>(const Tuple& other) const {
            for(std::size_t i = 0; i < arity; ++i) {
                if (data[i] > other.data[i]) return true;
                if (data[i] < other.data[i]) return false;
            }
            return false;
        }

        // allow tuples to be printed
        friend std::ostream& operator<<(std::ostream& out, const Tuple& tuple) {
            if (arity == 0) return out << "[]";
            out << "[";
            for(std::size_t i =0; i<(std::size_t)(arity-1); ++i) {
                out << tuple.data[i]; out << ",";
            }
            return out << tuple.data[arity-1] << "]";
        }
    };

} // end namespace ram
} // end of namespace souffle


// -- add hashing support --

namespace std {

    template<typename Domain, std::size_t arity>
    struct hash<souffle::ram::Tuple<Domain,arity>> {
        size_t operator()(const souffle::ram::Tuple<Domain,arity>& value) const {
            std::hash<Domain> hash;
            size_t res = 0;
            for(unsigned i=0; i<arity; i++) {
                // from boost hash combine
                res ^= hash(value[i]) + 0x9e3779b9 + (res << 6) + (res >> 2);
            }
            return res;
        }
    };

}
