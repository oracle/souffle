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
 * @file matching_test.h
 *
 * Test cases for the computation of optimal indices.
 *
 ***********************************************************************/

#include "test.h"

#include "../RamAutoIndex.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include <limits>
#include <random>
#include <functional>


using namespace std;
using namespace souffle;

class TestAutoIndex : public RamAutoIndex {
public:
    /** returns number of unique matchings */
    int getNumMatchings(){
       return matching.getNumMatchings();
    }
};


typedef set<SearchColumns> Nodes;


TEST(Matching, StaticTest_1) {
    TestAutoIndex order;
    Nodes nodes;

    order.addSearch(31);
    nodes.insert(31);
    order.addSearch(23);
    nodes.insert(23);
    order.addSearch(15);
    nodes.insert(15);
    order.addSearch(7);
    nodes.insert(7);
    order.addSearch(5);
    nodes.insert(5);
    order.addSearch(3);
    nodes.insert(3);
    order.addSearch(1);
    nodes.insert(1);

    order.solve();
    int num = order.getNumMatchings();

    EXPECT_EQ(num, 5);
}



TEST(Matching, StaticTest_2) {

    TestAutoIndex order;
    Nodes nodes;

    order.addSearch(121);
    nodes.insert(121);
    order.addSearch(104);
    nodes.insert(104);
    order.addSearch(53);
    nodes.insert(53);
    order.addSearch(49);
    nodes.insert(49);
    order.addSearch(39);
    nodes.insert(39);
    order.addSearch(33);
    nodes.insert(33);
    order.addSearch(32);
    nodes.insert(32);
    order.addSearch(23);
    nodes.insert(23);
    order.addSearch(11);
    nodes.insert(11);
    order.addSearch(7);
    nodes.insert(7);


    order.solve();
    int num = order.getNumMatchings();

    EXPECT_EQ(num, 5);
}


