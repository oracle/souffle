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

#include "AstClause.h"
#include "AstParserUtils.h"
#include "test.h"

namespace souffle {

namespace test {

TEST(RuleBody, Basic) {
    RuleBody body;

    // start with an A
    auto a = RuleBody::atom(new AstAtom("A"));
    EXPECT_EQ("A()", toString(a));

    a.conjunct(RuleBody::atom(new AstAtom("B")));
    EXPECT_EQ("A(),B()", toString(a));

    a.disjunct(RuleBody::atom(new AstAtom("C")));
    EXPECT_EQ("A(),B();C()", toString(a));
}

TEST(RuleBody, Negation) {
    RuleBody body = RuleBody::getTrue();

    RuleBody AB = RuleBody::getTrue();
    AB.conjunct(RuleBody::atom(new AstAtom("A")));
    AB.conjunct(RuleBody::atom(new AstAtom("B")));
    EXPECT_EQ("A(),B()", toString(AB));

    RuleBody CD = RuleBody::getTrue();
    CD.conjunct(RuleBody::atom(new AstAtom("C")));
    CD.conjunct(RuleBody::atom(new AstAtom("D")));
    EXPECT_EQ("C(),D()", toString(CD));

    RuleBody EF = RuleBody::getTrue();
    EF.conjunct(RuleBody::atom(new AstAtom("E")));
    EF.conjunct(RuleBody::atom(new AstAtom("F")));
    EXPECT_EQ("E(),F()", toString(EF));

    RuleBody full = RuleBody::getFalse();
    full.disjunct(std::move(AB));
    full.disjunct(std::move(CD));
    full.disjunct(std::move(EF));
    EXPECT_EQ("A(),B();C(),D();E(),F()", toString(full));

    full.negate();
    EXPECT_EQ(
            "!A(),!C(),!E();!A(),!C(),!F();!A(),!D(),!E();!A(),!D(),!F();!B(),!C(),!E();!B(),!C(),!F();!B(),!"
            "D(),!E();!B(),!D(),!F()",
            toString(full));

    full.negate();
    EXPECT_EQ("A(),B();C(),D();E(),F()", toString(full));
}

TEST(RuleBody, ClauseBodyExtraction) {
    RuleBody body = RuleBody::getTrue();

    RuleBody AB = RuleBody::getTrue();
    AB.conjunct(RuleBody::atom(new AstAtom("A")));
    AB.conjunct(RuleBody::atom(new AstAtom("B")));
    EXPECT_EQ("A(),B()", toString(AB));

    RuleBody CD = RuleBody::getTrue();
    CD.conjunct(RuleBody::atom(new AstAtom("C")));
    CD.conjunct(RuleBody::atom(new AstAtom("D")));
    EXPECT_EQ("C(),D()", toString(CD));

    RuleBody EF = RuleBody::getTrue();
    EF.conjunct(RuleBody::atom(new AstAtom("E")));
    EF.conjunct(RuleBody::atom(new AstAtom("F")));
    EXPECT_EQ("E(),F()", toString(EF));

    RuleBody full = RuleBody::getFalse();
    full.disjunct(std::move(AB));
    full.disjunct(std::move(CD));
    full.disjunct(std::move(EF));
    EXPECT_EQ("A(),B();C(),D();E(),F()", toString(full));

    // extract the clause
    auto list = full.toClauseBodies();
    EXPECT_EQ(3, list.size());

    EXPECT_EQ(" :- \n   A(),\n   B().", toString(*list[0]));
    EXPECT_EQ(" :- \n   C(),\n   D().", toString(*list[1]));
    EXPECT_EQ(" :- \n   E(),\n   F().", toString(*list[2]));

    // free the clauses
    for (const auto& cur : list) {
        delete cur;
    }
}

}  // end namespace test
}  // end namespace souffle
