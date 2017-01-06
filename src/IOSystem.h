/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */


/************************************************************************
 *
 * @file IOSystem.h
 *
 ***********************************************************************/

#pragma once

#include <istream>
#include <memory>
#include <ostream>
#include <string>

#include "ReadStream.h"
#include "ReadStreamCSV.h"
#include "SymbolMask.h"
#include "SymbolTable.h"
#include "WriteStream.h"
#include "WriteStreamCSV.h"

namespace souffle {

class IOSystem {
public:
    static IOSystem& getInstance() {
        static IOSystem singleton;
        return singleton;
    }
    std::unique_ptr<ReadStream> getCSVReader(std::istream& in, const SymbolMask& format,
            SymbolTable &symtab, char delimiter = '\t') {
        return std::unique_ptr<ReadStreamCSV>(new ReadStreamCSV(in, format, symtab, delimiter));
    }
    std::unique_ptr<ReadStream> getCSVReader(const std::string& filename, const SymbolMask& symbolMask,
            SymbolTable &symbolTable, char delimiter = '\t') {
        return std::unique_ptr<ReadFileCSV>(new ReadFileCSV(filename, symbolMask, symbolTable, delimiter));
    }
    std::unique_ptr<WriteStream> getCSVWriter(std::ostream& out, const SymbolMask& format,
            const SymbolTable &symtab, char delimiter = '\t') {
        return std::unique_ptr<WriteStreamCSV>(new WriteStreamCSV(out, format, symtab, delimiter));
    }
    std::unique_ptr<WriteStream> getCSVWriter(const std::string& filename, const SymbolMask& format,
            const SymbolTable &symtab, char delimiter = '\t') {
        return std::unique_ptr<WriteFileCSV>(new WriteFileCSV(filename, format, symtab, delimiter));
    }
    std::unique_ptr<WriteStream> getCoutCSVWriter(const std::string& filename, const SymbolMask& format,
            const SymbolTable &symtab, char delimiter = '\t') {
        return std::unique_ptr<WriteCoutCSV>(new WriteCoutCSV(filename, format, symtab, delimiter));
    }
    ~IOSystem() {}
private:
    IOSystem() {};
};

} /* namespace souffle */
