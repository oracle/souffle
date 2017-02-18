/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file BinaryFunctorOps.h
 *
 * Defines binary functor operators for AST and RAM
 *
 ***********************************************************************/
#pragma once

#include <cassert>
#include <iostream>

namespace souffle {

/**
 * Binary Functor Operators
 */
enum class BinaryOp {
    __UNDEFINED__,  // undefined operator
    ADD,            // addition
    SUB,            // subtraction
    MUL,            // multiplication
    DIV,            // division
    EXP,            // exponent
    MOD,            // modulus
    BAND,           // bitwise and
    BOR,            // bitwise or
    BXOR,           // bitwise exclusive or
    LAND,           // logical and
    LOR,            // logical or
    CAT,            // string concatenation
};

/**
 * Converts operator to its symbolic representation
 */
inline std::string getSymbolForBinaryOp(BinaryOp op) {
    switch (op) {
        case BinaryOp::ADD:
            return "+";
        case BinaryOp::SUB:
            return "-";
        case BinaryOp::MUL:
            return "*";
        case BinaryOp::DIV:
            return "/";
        case BinaryOp::EXP:
            return "^";
        case BinaryOp::MOD:
            return "%";
        case BinaryOp::BAND:
            return "band";
        case BinaryOp::BOR:
            return "bor";
        case BinaryOp::BXOR:
            return "bxor";
        case BinaryOp::LAND:
            return "land";
        case BinaryOp::LOR:
            return "lor";
        case BinaryOp::CAT:
            return "cat";
        default:
            break;
    }
    assert(false && "Unsupported Operator!");
    return "?";
}

/**
 * Converts symbolic representation of an operator to the operator
 */
inline BinaryOp getBinaryOpForSymbol(const std::string& symbol) {
    if (symbol == "+") return BinaryOp::ADD;
    if (symbol == "-") return BinaryOp::SUB;
    if (symbol == "*") return BinaryOp::MUL;
    if (symbol == "/") return BinaryOp::DIV;
    if (symbol == "^") return BinaryOp::EXP;
    if (symbol == "%") return BinaryOp::MOD;
    if (symbol == "band") return BinaryOp::BAND;
    if (symbol == "bor") return BinaryOp::BOR;
    if (symbol == "bxor") return BinaryOp::BXOR;
    if (symbol == "land") return BinaryOp::LAND;
    if (symbol == "lor") return BinaryOp::LOR;
    if (symbol == "cat") return BinaryOp::CAT;
    std::cout << "Unrecognised operator: " << symbol << "\n";
    assert(false && "Unsupported Operator!");
    return BinaryOp::__UNDEFINED__;
}

/**
 * Determines whether the given operator has a numeric return value.
 */
inline bool isNumericBinaryOp(const BinaryOp op) {
    switch (op) {
        case BinaryOp::ADD:
        case BinaryOp::SUB:
        case BinaryOp::MUL:
        case BinaryOp::DIV:
        case BinaryOp::EXP:
        case BinaryOp::BAND:
        case BinaryOp::BOR:
        case BinaryOp::BXOR:
        case BinaryOp::LAND:
        case BinaryOp::LOR:
        case BinaryOp::MOD:
            return true;
        case BinaryOp::CAT:
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
inline bool isSymbolicBinaryOp(const BinaryOp op) {
    return !isNumericBinaryOp(op);
}

/**
 * Determines whether an argument has a number value.
 */
inline bool binaryOpAcceptsNumbers(int arg, const BinaryOp op) {
    assert(arg >= 0 && arg < 2 && "argument out of range"); 
    switch (op) {
        case BinaryOp::ADD:
        case BinaryOp::SUB:
        case BinaryOp::MUL:
        case BinaryOp::DIV:
        case BinaryOp::EXP:
        case BinaryOp::BAND:
        case BinaryOp::BOR:
        case BinaryOp::BXOR:
        case BinaryOp::LAND:
        case BinaryOp::LOR:
        case BinaryOp::MOD:
            return true;
        case BinaryOp::CAT:
            return false;
        default:
            break;
    }
    assert(false && "Uncovered case!");
    return false;
}

/**
 * Determines whether an argument has a symbolic value
 */
inline bool binaryOpAcceptsSymbols(int arg, const BinaryOp op) {
    return !binaryOpAcceptsNumbers(arg, op);
}

}  // end of namespace souffle
