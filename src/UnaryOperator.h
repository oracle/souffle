/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file UnaryOperator.h
 *
 * Defines unary operators and relational operators.
 *
 ***********************************************************************/
#pragma once

#include <cassert>

namespace souffle {

/**
 * Unary Relational Operators
 */
enum class UnaryRelOp {
    __UNDEFINED__
};

/**
 * Returns the corresponding symbol for the given relational operator.
 */
inline std::string getSymbolForUnaryRelOp(UnaryRelOp op) {
    switch(op) {
        case UnaryRelOp::__UNDEFINED__: break;
        // TODO
    }
    assert(false && "Unsupported Operator!");
    return "?";
}

/**
 * Returns the corresponding relation operator for the given symbol.
 */
inline UnaryRelOp getUnaryRelOpForSymbol(const std::string &symbol) {
    std::cout << "Unrecognised operator: " << symbol << "\n";
    assert(false && "Unsupported Operator!");
    return UnaryRelOp::__UNDEFINED__;
}

/**
 * Returns whether the given relational operator has numeric operands.
 */
inline bool isNumericUnaryRelOp(const UnaryRelOp op) {
    switch(op) {
        case UnaryRelOp::__UNDEFINED__: break;
        // TODO
    }
    assert(false && "Uncovered case!");
    return false;
}

/**
 * Returns whether the given operator has symbolic operands.
 */
inline bool isSymbolicUnaryRelOp(const UnaryRelOp op) {
    return !isNumericUnaryRelOp(op);
}

/**
 * Unary Operators
 */
enum class UnaryOp {
    __UNDEFINED__,
    ORDINAL,
    NEGATION,
    BNOT,
    LNOT
};

/**
 * Returns the corresponding symbol for the given relational operator.
 */
inline std::string getSymbolForUnaryOp(UnaryOp op) {
    switch(op) {
    case UnaryOp::__UNDEFINED__: break;
    case UnaryOp::ORDINAL : return "ord";
    case UnaryOp::NEGATION : return "-";
    case UnaryOp::BNOT : return "bnot";
    case UnaryOp::LNOT : return "lnot";
    }
    assert(false && "Unsupported Operator!");
    return "?";
}

/**
 * Returns the corresponding operator for the given symbol.
 */
inline UnaryOp getUnaryOpForSymbol(const std::string &symbol) {
    if (symbol == "ord") return  UnaryOp::ORDINAL;
    if (symbol == "-") return  UnaryOp::NEGATION;
    if (symbol == "bnot") return  UnaryOp::BNOT;
    if (symbol == "lnot") return  UnaryOp::LNOT;
    std::cout << "Unrecognised operator: " << symbol << "\n";
    assert(false && "Unsupported Operator!");
    return UnaryOp::__UNDEFINED__;
}

/**
 * Returns whether the given operator has numeric operands.
 */
inline bool isNumericUnaryOp(const UnaryOp op) {
    switch(op) {
    case UnaryOp::__UNDEFINED__: break;
    case UnaryOp::ORDINAL:
    case UnaryOp::NEGATION:
    case UnaryOp::BNOT:
    case UnaryOp::LNOT: return true;
    }
    assert(false && "Uncovered case!");
    return false;
}

/**
 * Returns whether the given operator has symbolic operands.
 */
inline bool isSymbolicUnaryOp(const UnaryOp op) {
    return !isNumericUnaryOp(op);
}

inline bool unaryOpAcceptsNumbers(const UnaryOp op) {
    switch (op) {
    case UnaryOp::__UNDEFINED__: break;
    case UnaryOp::NEGATION:
    case UnaryOp::BNOT:
    case UnaryOp::LNOT:
    case UnaryOp::ORDINAL: return false;
    }
    assert(false && "Unsupported operator encountered!");
    return false;
}

inline bool unaryOpAcceptsSymbols(const UnaryOp op) {
    switch (op) {
    case UnaryOp::__UNDEFINED__: break;
    case UnaryOp::NEGATION:
    case UnaryOp::BNOT:
    case UnaryOp::LNOT: return false;
    case UnaryOp::ORDINAL: return true;
    }
    assert(false && "Unsupported operator encountered!");
    return false;
}

} // end of namespace souffle

