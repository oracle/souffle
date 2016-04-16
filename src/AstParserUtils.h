/*
 * Copyright (c) 2013, Oracle and/or its affiliates. All Rights reserved
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
 * @file AstClause.h
 *
 * Defines class Clause that represents rules including facts, predicates, and
 * queries in a Datalog program.
 *
 ***********************************************************************/

#pragma once

#include <vector>
#include <memory>

#include "AstLiteral.h"
#include "Util.h"

namespace souffle {

class RuleBody {

	// a struct to represent literals
	struct literal {

		// whether this literal is negated or not
		bool negated;

		// the atom referenced by tis literal
		std::unique_ptr<AstLiteral> atom;

		literal clone() const {
			return literal({
				negated,
				std::unique_ptr<AstLiteral>(atom->clone())
			});
		}
	};

	using clause = std::vector<literal>;
	std::vector<clause> dnf;

public:

	RuleBody() {}

	void negate();

	void conjunct(RuleBody&& other);

	void disjunct(RuleBody&& other);

	std::vector<AstClause*> toClauseBodies() const;

	// -- factory functions --

	static RuleBody getTrue();

	static RuleBody getFalse();

	static RuleBody atom(AstAtom* atom);

	static RuleBody constraint(AstConstraint* constraint);

	friend std::ostream& operator<<(std::ostream& out, const RuleBody& body);

private:

	static bool equal(const literal& a, const literal& b);

	static bool equal(const clause& a, const clause& b);

	static bool isSubsetOf(const clause& a, const clause& b);

	static void insert(clause& cl, literal&& lit);

	static void insert(std::vector<clause>& cnf, clause&& cls);

};

} // end of namespace souffle

