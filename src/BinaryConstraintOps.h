/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file BinaryConstraintOps.h
 *
 * Defines binary constraint operators for AST & RAM
 *
 ***********************************************************************/
#pragma once

#include <cassert>
#include <iostream>

namespace souffle {

/******************************************
 * Helper Functions for Binary Constraints
 ******************************************/

/**
 * Binary Constraint Operators
 */
enum class BinaryRelOp {
    __UNDEFINED__,  // undefined operator
    EQ,             // equivalence of two values
    NE,             // whether two values are different
    LT,             // less-than
    LE,             // less-than-or-equal-to
    GT,             // greater-than
    GE,             // greater-than-or-equal-to
    MATCH,          // matching string
    CONTAINS,       // whether a sub-string is contained in a string
    NOT_MATCH,      // not matching string
    NOT_CONTAINS    // whether a sub-string is not contained in a string
};

/**
 * Negated Constraint Operator
 * Each opeprator requires a negated operator which is
 * necessary for the expansion of complex rule bodies with disjunction and negation.
 */
inline BinaryRelOp negate(BinaryRelOp op) {
    switch (op) {
        case BinaryRelOp::EQ:
            return BinaryRelOp::NE;
        case BinaryRelOp::NE:
            return BinaryRelOp::EQ;
        case BinaryRelOp::LT:
            return BinaryRelOp::GE;
        case BinaryRelOp::LE:
            return BinaryRelOp::GT;
        case BinaryRelOp::GE:
            return BinaryRelOp::LT;
        case BinaryRelOp::GT:
            return BinaryRelOp::LE;
        case BinaryRelOp::MATCH:
            return BinaryRelOp::NOT_MATCH;
        case BinaryRelOp::NOT_MATCH:
            return BinaryRelOp::MATCH;
        case BinaryRelOp::CONTAINS:
            return BinaryRelOp::NOT_CONTAINS;
        case BinaryRelOp::NOT_CONTAINS:
            return BinaryRelOp::CONTAINS;
        default:
            break;
    }
    assert(false && "Unsupported Operator!");
    return op;
}

/**
 * Converts operator to its symbolic representation
 */
inline std::string getSymbolForBinaryRelOp(BinaryRelOp op) {
    switch (op) {
        case BinaryRelOp::EQ:
            return "=";
        case BinaryRelOp::NE:
            return "!=";
        case BinaryRelOp::LT:
            return "<";
        case BinaryRelOp::LE:
            return "<=";
        case BinaryRelOp::GT:
            return ">";
        case BinaryRelOp::GE:
            return ">=";
        case BinaryRelOp::MATCH:
            return "match";
        case BinaryRelOp::CONTAINS:
            return "contains";
        case BinaryRelOp::NOT_MATCH:
            return "not_match";
        case BinaryRelOp::NOT_CONTAINS:
            return "not_contains";
        default:
            break;
    }
    assert(false && "Unsupported Operator!");
    return "?";
}

/**
 * Converts symbolic representation of an operator to the operator
 */
inline BinaryRelOp getBinaryRelOpForSymbol(const std::string& symbol) {
    if (symbol == "=") return BinaryRelOp::EQ;
    if (symbol == "!=") return BinaryRelOp::NE;
    if (symbol == "<") return BinaryRelOp::LT;
    if (symbol == "<=") return BinaryRelOp::LE;
    if (symbol == ">=") return BinaryRelOp::GE;
    if (symbol == ">") return BinaryRelOp::GT;
    if (symbol == "match") return BinaryRelOp::MATCH;
    if (symbol == "contains") return BinaryRelOp::CONTAINS;
    if (symbol == "not_match") return BinaryRelOp::NOT_MATCH;
    if (symbol == "not_contains") return BinaryRelOp::NOT_CONTAINS;
    std::cout << "Unrecognised operator: " << symbol << "\n";
    assert(false && "Unsupported Operator!");
    return BinaryRelOp::__UNDEFINED__;
}

/**
 * Helper Functions for Binary Functors
 */

/**
 * Determines whether arguments of constraint are numeric
 */
inline bool isNumericBinaryRelOp(const BinaryRelOp op) {
    switch (op) {
        case BinaryRelOp::EQ:
        case BinaryRelOp::NE:
        case BinaryRelOp::LT:
        case BinaryRelOp::LE:
        case BinaryRelOp::GE:
        case BinaryRelOp::GT:
            return true;

        case BinaryRelOp::MATCH:
        case BinaryRelOp::NOT_MATCH:
        case BinaryRelOp::CONTAINS:
        case BinaryRelOp::NOT_CONTAINS:
            return false;

        default:
            break;
    }
    assert(false && "Uncovered case!");
    return false;
}

/**
 * Determines whether arguments of constraint are numeric
 */
inline bool isSymbolicBinaryRelOp(const BinaryRelOp op) {
    return !isNumericBinaryRelOp(op);
}

}  // end of namespace souffle
