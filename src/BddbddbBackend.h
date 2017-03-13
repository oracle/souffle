/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file BddbddbBackend.h
 *
 * Declares the interface for bddbddb backend.
 *
 ***********************************************************************/
#pragma once

#include <exception>
#include <ostream>
#include <string>

namespace souffle {

class AstTranslationUnit;

/**
 * Converts the given souffle-datalog translation unit into bdddbddb input code
 * and writes the result into the given output stream.
 *
 * @param out the stream to write the result to
 * @param translationUnit the translation unit to be converted
 * @throws UnsupportedConstructException if constructs that can not be expressed
 *        within bddbddb have been encountered
 */
void toBddbddb(std::ostream& out, const AstTranslationUnit& translationUnit);

/**
 * The kind of exception to be raised if an error in the conversion to a
 * bddbddb input program has been encountered.
 */
class UnsupportedConstructException : public std::exception {
    std::string msg;

public:
    UnsupportedConstructException(const std::string& msg) : msg(msg) {}

    const char* what() const noexcept override {
        return msg.c_str();
    }
};

}  // end of namespace souffle
