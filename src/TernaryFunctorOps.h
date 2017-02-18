/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file TernaryFunctorOps.h
 *
 * Defines ternary functor operators for AST and RAM
 *
 ***********************************************************************/
#pragma once

#include <cassert>
#include <iostream>

namespace souffle {

/**
 * Ternary Functor Operators
 */
enum class TernaryOp {
    __UNDEFINED__,  // undefined operator
    SUBSTR,         // addition
};

/**
 * Converts operator to its symbolic representation
 */
inline std::string getSymbolForTernaryOp(TernaryOp op) {
    switch (op) {
        case TernaryOp::SUBSTR:
            return "substr";
        default:
            break;
    }
    assert(false && "Unsupported Operator!");
    return "?";
}

/**
 * Converts symbolic representation of an operator to the operator
 */
inline TernaryOp getTernaryOpForSymbol(const std::string& symbol) {
    if (symbol == "substr") return TernaryOp::SUBSTR;
    std::cout << "Unrecognised operator: " << symbol << "\n";
    assert(false && "Unsupported Operator!");
    return TernaryOp::__UNDEFINED__;
}

/**
 * Determines whether the given operator has a numeric return value.
 */
inline bool isNumericTernaryOp(const TernaryOp op) {
    switch (op) {
        case TernaryOp::SUBSTR:
            return false;
        default:
            break;
    }
    assert(false && "Uncovered case!");
    return false;
}

/**
 * Determines whether the operator has a symbolic return value.
 */
inline bool isSymbolicTernaryOp(const TernaryOp op) {
    return !isNumericTernaryOp(op);
}

/**
 * Determines whether an argument has a number value.
 */
inline bool ternaryOpAcceptsNumbers(int arg, const TernaryOp op) {
    assert(arg >= 0 && arg < 3 && "argument out of range");
    switch (op) {
        case TernaryOp::SUBSTR:
            return arg == 1 || arg == 2;
        default:
            break;
    }
    assert(false && "Uncovered case!");
    return false;
}

/**
 * Determines whether an argument has a symbolic value
 */
inline bool ternaryOpAcceptsSymbols(int arg, const TernaryOp op) {
    return !ternaryOpAcceptsNumbers(arg, op);
}

}  // end of namespace souffle
