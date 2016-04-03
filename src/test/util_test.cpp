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
 * @file table_test.h
 *
 * Test cases for the Table data structure.
 *
 ***********************************************************************/

#include "test.h"

#include "Util.h"

using namespace std;

TEST(Util, toString) {

    EXPECT_EQ("12", toString(12));
    EXPECT_EQ("Hello", toString("Hello"));
}

TEST(Util, toVector) {
    EXPECT_EQ("[1,2,3]", toString(toVector(1,2,3)));
    EXPECT_EQ("[]", toString(toVector<int>()));
}

TEST(Util, printVector) {

    vector<int> v;

    EXPECT_EQ("[]", toString(v));
    v.push_back(12);
    EXPECT_EQ("[12]", toString(v));
    v.push_back(14);
    EXPECT_EQ("[12,14]", toString(v));
}

TEST(Util, printSet) {

    set<int> v;

    EXPECT_EQ("{}", toString(v));
    v.insert(12);
    EXPECT_EQ("{12}", toString(v));
    v.insert(14);
    EXPECT_EQ("{12,14}", toString(v));

}


TEST(Util, printMap) {

    map<int,string> m;

    EXPECT_EQ("{}", toString(m));
    m[12] = "Hello";
    EXPECT_EQ("{12->Hello}", toString(m));
    m[14] = "World";
    EXPECT_EQ("{12->Hello,14->World}", toString(m));

}

TEST(Util, LambdaTraits) {

    auto lambda = [](int x)->bool { return true; };

    EXPECT_EQ(typeid(bool).name(), typeid(lambda_traits<decltype(lambda)>::result_type).name());
    EXPECT_EQ(typeid(int).name(), typeid(lambda_traits<decltype(lambda)>::arg0_type).name());

}

TEST(Util, NullStream) {

    NullStream nullstream;

    std::ostream* out;
    out = &nullstream;
    (*out) << "Hello World!\n";
}

