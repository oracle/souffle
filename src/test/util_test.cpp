/*
 * Souffle version 0.0.0
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - souffle/LICENSE
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
using namespace souffle;

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

