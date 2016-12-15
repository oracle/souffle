/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */


/************************************************************************
 *
 * @file ReadStreamCSV.h
 *
 ***********************************************************************/

#pragma once

#include "ReadStream.h"

#include "SymbolMask.h"
#include "SymbolTable.h"

#include <fstream>
#include <memory>
#include <string>

namespace souffle {

class ReadStreamCSV : public ReadStream {
public:
    ReadStreamCSV(std::istream& in, const SymbolMask& symbolMask, SymbolTable &symbolTable,
            char delimiter = '\t') : delimiter(delimiter),
            file(in),
            lineNumber(0),
            symbolMask(symbolMask),
            symbolTable(symbolTable) {};
    virtual bool isReadable() {
        return file.good();
    }
    /**
     * @return true if another tuple may be read.
     */
    virtual bool hasNextTuple();
    /**
     * Read and return the next tuple.
     *
     * Returns nullptr if no tuple was readable.
     * @return
     */
    virtual std::unique_ptr<RamDomain[]> readNextTuple();
    virtual ~ReadStreamCSV() {};
private:
    const char delimiter;
    std::istream& file;
    size_t lineNumber;
    const SymbolMask& symbolMask;
    SymbolTable& symbolTable;
};

} /* namespace souffle */
