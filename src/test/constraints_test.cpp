/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file type_system_test.h
 *
 * Tests souffle's type system operations.
 *
 ***********************************************************************/

#include "Constraints.h"
#include "Util.h"
#include "test.h"

#include <set>
#include <string>

using namespace std;

namespace souffle {

namespace test {

TEST(Constraints, Basic) {
    typedef Variable<string, set_property_space<int>> Vars;

    Vars A("A");
    Vars B("B");

    Problem<Vars> p;

    // check empty problem
    EXPECT_EQ("{}", toString(p));
    EXPECT_EQ("{}", toString(p.solve()));

    // add a constraint
    p.add(sub(A, B));
    EXPECT_EQ("{\n\tA ⊑ B\n}", toString(p));
    EXPECT_EQ("{A->{},B->{}}", toString(p.solve()));

    // add another constraints
    std::set<int> s = {1, 2};
    p.add(sub(s, A));
    EXPECT_EQ("{\n\tA ⊑ B,\n\t{1,2} ⊑ A\n}", toString(p));
    EXPECT_EQ("{A->{1,2},B->{1,2}}", toString(p.solve()));

    // and one more
    std::set<int> s2 = {3};
    p.add(sub(s2, B));
    EXPECT_EQ("{\n\tA ⊑ B,\n\t{1,2} ⊑ A,\n\t{3} ⊑ B\n}", toString(p));
    EXPECT_EQ("{A->{1,2},B->{1,2,3}}", toString(p.solve()));
}

}  // end namespace test
}  // end namespace souffle
