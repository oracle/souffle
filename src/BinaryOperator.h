/*
 * Copyright (c) 2015, Oracle and/or its affiliates.
 *
 * All rights reserved.
 */

/************************************************************************
 *
 * @file BinaryOperator.h
 *
 * Defines binary operators and relational operators.
 *
 ***********************************************************************/
#pragma once

#include <cassert>

/**
 * Binary Relational Operators
 */
enum class BinaryRelOp {
    EQ,      // equivalence of two values
    NE,      // whether two values are different
    LT,      // less-than
    LE,      // less-than-or-equal-to
    GT,      // greater-than
    GE,      // greater-than-or-equal-to
    MATCH,   // matching string
    CONTAINS // whether a sub-string is contained in a string
};

/**
 * Returns the corresponding symbol for the given relational operator.
 */
inline std::string getSymbolForBinaryRelOp(BinaryRelOp op) {
    switch(op) {
    case BinaryRelOp::EQ : return "=";
    case BinaryRelOp::NE : return "!=";
    case BinaryRelOp::LT : return "<";
    case BinaryRelOp::LE : return "<=";
    case BinaryRelOp::GT : return ">";
    case BinaryRelOp::GE : return ">=";
    case BinaryRelOp::MATCH : return "match";
    case BinaryRelOp::CONTAINS : return "contains";
    }
    assert(false && "Unsupported Operator!");
    return "?";
}

/**
 * Returns the corresponding relation operator for the given symbol.
 */
inline BinaryRelOp getBinaryRelOpForSymbol(const std::string &symbol) {
    if (symbol == "=") return BinaryRelOp::EQ;
    if (symbol == "!=") return BinaryRelOp::NE;
    if (symbol == "<") return BinaryRelOp::LT;
    if (symbol == "<=") return BinaryRelOp::LE;
    if (symbol == ">=") return BinaryRelOp::GE;
    if (symbol == ">") return BinaryRelOp::GT;
    if (symbol == "match") return BinaryRelOp::MATCH;
    if (symbol == "contains") return BinaryRelOp::CONTAINS;
    std::cout << "Unrecognised operator: " << symbol << "\n";
    assert(false && "Unsupported Operator!");
    return BinaryRelOp::EQ;
}

/**
 * Returns whether the given relational operator has numeric operands.
 */
inline bool isNumericBinaryRelOp(const BinaryRelOp op) {
    switch(op) {
    case BinaryRelOp::EQ:
    case BinaryRelOp::NE:
    case BinaryRelOp::LT:
    case BinaryRelOp::LE:
    case BinaryRelOp::GE:
    case BinaryRelOp::GT: return true;
    case BinaryRelOp::MATCH: return false;
    case BinaryRelOp::CONTAINS: return false;
    }
    assert(false && "Uncovered case!");
    return false;
}

/**
 * Returns whether the given operator has symbolic operands.
 */
inline bool isSymbolicBinaryRelOp(const BinaryRelOp op) {
    return !isNumericBinaryRelOp(op);
}

/**
 * Binary Operators
 */
enum class BinaryOp {
    ADD, // addition
    SUB, // subtraction
    MUL, // multiplication
    DIV, // division
    EXP, // exponent
    MOD, // modulus
    CAT, // string concatenation
};

/**
 * Returns the corresponding symbol for the given relational operator.
 */
inline std::string getSymbolForBinaryOp(BinaryOp op) {
    switch(op) {
    case BinaryOp::ADD : return "+";
    case BinaryOp::SUB : return "-";
    case BinaryOp::MUL : return "*";
    case BinaryOp::DIV : return "/";
    case BinaryOp::EXP : return "^";
    case BinaryOp::MOD : return "%";
    case BinaryOp::CAT : return "cat";
    }
    assert(false && "Unsupported Operator!");
    return "?";
}

/**
 * Returns the corresponding operator for the given symbol.
 */
inline BinaryOp getBinaryOpForSymbol(const std::string &symbol) {
    if (symbol == "+") return BinaryOp::ADD;
    if (symbol == "-") return BinaryOp::SUB;
    if (symbol == "*") return BinaryOp::MUL;
    if (symbol == "/") return BinaryOp::DIV;
    if (symbol == "^") return BinaryOp::EXP;
    if (symbol == "%") return BinaryOp::MOD;
    if (symbol == "cat") return BinaryOp::CAT;
    std::cout << "Unrecognised operator: " << symbol << "\n";
    assert(false && "Unsupported Operator!");
    return BinaryOp::ADD;
}

/**
 * Returns whether the given operator has numeric operands.
 */
inline bool isNumericBinaryOp(const BinaryOp op) {
    switch(op) {
    case BinaryOp::ADD:
    case BinaryOp::SUB:
    case BinaryOp::MUL:
    case BinaryOp::DIV:
    case BinaryOp::EXP:
    case BinaryOp::MOD: return true;
    case BinaryOp::CAT: return false;
    }
    assert(false && "Uncovered case!");
    return false;
}

/**
 * Returns whether the given operator has symbolic operands.
 */
inline bool isSymbolicBinaryOp(const BinaryOp op) {
    return !isNumericBinaryOp(op);
}
