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
    ReadStreamCSV(const std::string &fname, const SymbolMask& symbolMask, SymbolTable &symbolTable,
            char delimiter = '\t') : file(std::ifstream(fname, std::ifstream::in)), symbolMask(symbolMask),
            symbolTable(symbolTable), delimiter(delimiter), lineNumber(0) {};
    virtual bool isReadable() {
        return file.is_open();
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
    std::ifstream file;
    const SymbolMask& symbolMask;
    SymbolTable& symbolTable;
    const char& delimiter;
    size_t lineNumber;
};

} /* namespace souffle */
