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


#include <vector>
#include <memory>

#include "AstParserUtils.h"
#include "AstClause.h"

namespace souffle {

void RuleBody::negate() {

	RuleBody res = getTrue();

	for(const clause& cur : dnf) {

		RuleBody step = getFalse();

		for(const literal& lit : cur) {
			step.dnf.push_back(clause());
			clause& cl = step.dnf.back();
			cl.emplace_back(std::move(literal{!lit.negated, std::unique_ptr<AstLiteral>(lit.atom->clone())}));
		}

		res.conjunct(std::move(step));
	}

	dnf.swap(res.dnf);
}

void RuleBody::conjunct(RuleBody&& other) {

	std::vector<clause> res;

	for(const auto& clauseA : dnf) {
		for(const auto& clauseB : other.dnf) {

			// create a new clause in result
			clause cur;

			// it is the concatenation of the two clauses
			for(const auto& lit : clauseA) {
				cur.emplace_back(lit.clone());
			}
			for(const auto& lit : clauseB) {
				insert(cur,std::move(lit.clone()));
			}

			insert(res,std::move(cur));

		}
	}

	// update local dnf
	dnf.swap(res);
}

void RuleBody::disjunct(RuleBody&& other) {
	// append the clauses of the other body to this body
	for(auto& cur : other.dnf) {
		// insert clause
		insert(dnf, std::move(cur));
	}
}

std::vector<AstClause*> RuleBody::toClauseBodies() const {

	// collect clause results
	std::vector<AstClause*> bodies;
	for(const clause& cur : dnf) {
		bodies.push_back(new AstClause());
		AstClause& clause = *bodies.back();

		for(const literal& lit : cur) {
			// extract literal
			AstLiteral* base = lit.atom->clone();
			// negate if necessary
			if (lit.negated) {
				// negate
				if (AstAtom* atom = dynamic_cast<AstAtom*>(base)) {
					base = new AstNegation(std::unique_ptr<AstAtom>(atom));
					base->setSrcLoc(atom->getSrcLoc());
				} else if (AstConstraint* cstr = dynamic_cast<AstConstraint*>(base)) {
					cstr->negate();
				}
			}

			// add to result
			clause.addToBody(std::unique_ptr<AstLiteral>(base));
		}
	}

	// done
	return bodies;

}

// -- factory functions --

RuleBody RuleBody::getTrue() {
	RuleBody body;
	body.dnf.push_back(clause());
	return body;
}

RuleBody RuleBody::getFalse() {
	return RuleBody();
}

RuleBody RuleBody::atom(AstAtom* atom) {
	RuleBody body;
	body.dnf.push_back(clause());
	auto& clause = body.dnf.back();
	clause.push_back(literal());
	clause.back() = std::move(literal{false,std::unique_ptr<AstAtom>(atom)});
	return body;
}

RuleBody RuleBody::constraint(AstConstraint* constraint) {
	RuleBody body;
	body.dnf.push_back(clause());
	auto& clause = body.dnf.back();
	clause.push_back(literal());
	clause.back() = std::move(literal{false,std::unique_ptr<AstLiteral>(constraint)});
	return body;
}

std::ostream& operator<<(std::ostream& out, const RuleBody& body) {
	return out << join(body.dnf, ";", [](std::ostream& out, const RuleBody::clause& cur){
		out << join(cur, ",", [](std::ostream& out, const RuleBody::literal& l){
			if (l.negated) out << "!";
			out << *l.atom;
		});
	});
}

bool RuleBody::equal(const literal& a, const literal& b) {
	return a.negated == b.negated && *a.atom == *b.atom;
}

bool RuleBody::equal(const clause& a, const clause& b) {
	if (a.size() != b.size()) return false;
	for(std::size_t i = 0; i < a.size(); ++i) {
		bool found = false;
		for(std::size_t j = 0; !found && j<b.size(); ++j) {
			if (equal(a[i],b[j])) found = true;
		}
		if (!found) return false;
	}
	return true;
}

bool RuleBody::isSubsetOf(const clause& a, const clause& b) {
	if (a.size() > b.size()) return false;
	for(std::size_t i = 0; i < a.size(); ++i) {
		bool found = false;
		for(std::size_t j = 0; !found && j<b.size(); ++j) {
			if (equal(a[i],b[j])) found = true;
		}
		if (!found) return false;
	}
	return true;
}

void RuleBody::insert(clause& cl, literal&& lit) {
	for(const auto& cur : cl) {
		if (equal(cur, lit)) return;
	}
	cl.emplace_back(std::move(lit));
}

void RuleBody::insert(std::vector<clause>& cnf, clause&& cls) {
	for(const auto& cur : cnf) {
		if (isSubsetOf(cur,cls)) return;
	}
	std::vector<clause> res;
	for(auto& cur : cnf) {
		if (!isSubsetOf(cls, cur)) {
			res.emplace_back(std::move(cur));
		}
	}
	res.swap(cnf);
	cnf.emplace_back(std::move(cls));

}

} // end of namespace souffle

