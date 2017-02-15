/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file WriteStream.h
 *
 ***********************************************************************/

#pragma once

#include "RamTypes.h"
#include "SymbolMask.h"
#include "SymbolTable.h"

namespace souffle {

class WriteStream {
public:
    template <typename T>
    void writeAll(const T& relation) {
        for (const auto& current : relation) {
            writeNext(current);
        }
    }
    // TODO (mmcgr): change all the Tuple<RamDomain, arity> to std:array, then remove this.
    template <typename Tuple>
    void writeNext(Tuple tuple) {
        writeNextTuple(tuple.data);
    }
    virtual void writeNextTuple(const RamDomain* tuple) = 0;
    virtual ~WriteStream() {}
};

class WriteStreamFactory {
public:
    virtual std::unique_ptr<WriteStream> getWriter(const SymbolMask& symbolMask,
            const SymbolTable& symbolTable, const IODirectives& ioDirectives) = 0;
    virtual const std::string& getName() const = 0;
    virtual ~WriteStreamFactory() {}
};

template <>
void WriteStream::writeNext(const RamDomain* tuple) {
    writeNextTuple(tuple);
}

} /* namespace souffle */
