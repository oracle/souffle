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
 * @file AstArgument.cpp
 *
 * Define the classes Argument, Variable, and Constant to represent
 * variables and constants in literals. Variable and Constant are
 * sub-classes of class argument.
 *
 ***********************************************************************/

#include "AstArgument.h"

#include "AstLiteral.h"

std::vector<const AstNode*> AstAggregator::getChildNodes() const {
    auto res = AstArgument::getChildNodes();
    if (expr) res.push_back(expr.get());
    for(auto& cur : body) res.push_back(cur.get());
    return res;
}

AstAggregator* AstAggregator::clone() const {
    auto res = new AstAggregator(fun);
    res->expr = (expr) ? std::unique_ptr<AstArgument>(expr->clone()) : nullptr;
    for(const auto& cur : body) {
        res->body.push_back(std::unique_ptr<AstLiteral>(cur->clone()));
    }
    res->setSrcLoc(getSrcLoc());
    return res;
}

void AstAggregator::print(std::ostream& os) const {
    switch(fun) {
    case min: os << "min"; break;
    case max: os << "max"; break;
    case count: os << "count"; break;
    }

    if (expr) {
        os << " " << *expr;
    }

    os << " : ";
    if (body.size() > 1) {
        os << "{ ";
    }

    os << join(body, ", ", print_deref<std::unique_ptr<AstLiteral>>());

    if (body.size() > 1) {
        os << " }";
    }
}


