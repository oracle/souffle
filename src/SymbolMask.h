/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file SymbolMask.h
 *
 ***********************************************************************/

#pragma once

#include <ostream>
#include <vector>

namespace souffle {

class SymbolMask {
    std::vector<bool> mask;

public:
    SymbolMask(size_t arity) : mask(arity) {}

    SymbolMask(std::initializer_list<bool> symbolList) : mask(symbolList) {}

    size_t getArity() const {
        return mask.size();
    }

    bool isSymbol(size_t index) const {
        return index < getArity() && mask[index];
    }

    void setSymbol(size_t index, bool value = true) {
        if (index < getArity()) {
            mask[index] = value;
        }
    }

    void print(std::ostream& out) const {
        auto cur = mask.begin();
        if (cur == mask.end()) {
            return;
        }

        out << *cur++;
        for (; cur != mask.end(); ++cur) {
            out << ", " << *cur;
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const SymbolMask& mask) {
        mask.print(out);
        return out;
    }
};

} /* namespace souffle */
