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
 * @file compiled_ram_tuple_test.h
 *
 * Test cases for the RAM tuple data structure.
 *
 ***********************************************************************/

#include "test.h"

#include <iostream>
#include "CompiledRamTuple.h"

namespace souffle {
namespace ram {

    using namespace std;

    TEST(Tuple, Basic) {

        Tuple<int,3> t = {{ 1, 3, 2 }};

        EXPECT_EQ(3*sizeof(int), sizeof(t));

        std::cout << t << "\n";

        Tuple<int,2> t2 = {{ 1, 5 }};
        EXPECT_EQ(2*sizeof(int), sizeof(t2));
        std::cout << t2 << "\n";

    }

    TEST(Tuple, Assign) {

        Tuple<int,3> t1 = {{ 1, 2, 3 }};
        Tuple<int,3> t2 = {{ 3, 2, 1 }};

        Tuple<int,3> t3 = t1;

        EXPECT_NE(t1,t2);
        EXPECT_EQ(t1,t3);
        EXPECT_NE(t2,t3);

        t3 = t2;

        EXPECT_NE(t1,t2);
        EXPECT_NE(t1,t3);
        EXPECT_EQ(t2,t3);

    }


    TEST(Tuple, Compare) {

        Tuple<int,2> t1 = {{ 1, 2 }};
        Tuple<int,2> t2 = {{ 2, 1 }};

        EXPECT_LT(t1,t2);
    }

    TEST(Tuple, CompareSpeed) {

        // was used to evaluate various implementations of the == operator

        Tuple<int,2> t1 = {{1,2}};
        Tuple<int,2> t2 = {{2,1}};

        uint32_t res = 0;
        while(true) {
            res += !(t1 == t2);
            if (res & 0x10000000llu) break;
        }
        EXPECT_EQ(268435456llu, res);
    }

} // end namespace test
} // end namespace souffle
