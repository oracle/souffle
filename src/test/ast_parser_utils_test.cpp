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
 * @file type_system_test.h
 *
 * Tests souffle's type system operations.
 *
 ***********************************************************************/

#include "test.h"

#include "AstParserUtils.h"
#include "AstClause.h"

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
		EXPECT_EQ("!A(),!C(),!E();!A(),!C(),!F();!A(),!D(),!E();!A(),!D(),!F();!B(),!C(),!E();!B(),!C(),!F();!B(),!D(),!E();!B(),!D(),!F()", toString(full));

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
		EXPECT_EQ(3,list.size());

		EXPECT_EQ(" :- \n   A(),\n   B().",toString(*list[0]));
		EXPECT_EQ(" :- \n   C(),\n   D().",toString(*list[1]));
		EXPECT_EQ(" :- \n   E(),\n   F().",toString(*list[2]));


		// free the clauses
		for(const auto& cur : list) {
			delete cur;
		}
	}

} // end namespace test
} // end namespace souffle

