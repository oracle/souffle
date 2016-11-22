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
 * Unary Operators
 */
enum class UnaryOp {
    __UNDEFINED__,
    // used for AstUnaryOp
    ORDINAL,
    NEGATION,
    BNOT,
    LNOT,
    // used for RamUnaryOp
    COMPLEMENT,
    NEG,
    // NUMBER
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
    case UnaryOp::COMPLEMENT : return "complement";
    case UnaryOp::NEG : return "neg";
    // case UnaryOp::NUMBER : return "number";

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
    if (symbol == "complement") return UnaryOp::COMPLEMENT;
    if (symbol == "neg") return UnaryOp::NEG;
    // if (symbol == "number") return UnaryOp::NUMBER;
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

