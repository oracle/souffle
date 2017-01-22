/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file WriteStreamCSV.h
 *
 ***********************************************************************/

#pragma once

#include "WriteStream.h"

#include "SymbolMask.h"
#include "SymbolTable.h"

#include <memory>
#include <string>

namespace souffle {

class WriteStreamCSV: public WriteStream {
public:
    WriteStreamCSV(std::ostream& out, const SymbolMask& symbolMask, const SymbolTable &symbolTable,
            char delimiter = '\t') :
            delimiter(delimiter), out(out), symbolMask(symbolMask), symbolTable(
                    symbolTable) { }
    virtual void writeNextTuple(const RamDomain *tuple) {
        if (symbolMask.getArity() == 0) {
            out << "()\n";
            return;
        }

        if (symbolMask.isSymbol(0)) {
            out << symbolTable.resolve(tuple[0]);
        } else {
            out << (int32_t) tuple[0];
        }
        for (size_t col = 1; col < symbolMask.getArity(); ++col) {
            out << delimiter;
            if (symbolMask.isSymbol(col)) {
                std::string s = symbolTable.resolve(tuple[col]);
                out << s;
            } else {
                out << (int32_t) tuple[col];
            }
        }
        out << "\n";
    }


    virtual ~WriteStreamCSV() {}
private:
    const char delimiter;
    std::ostream& out;
    const SymbolMask& symbolMask;
    const SymbolTable& symbolTable;
};

class WriteFileCSV: public WriteStream {
public:
    WriteFileCSV(const std::string& filename, const SymbolMask& symbolMask, const SymbolTable &symbolTable,
            char delimiter = '\t') :
            file(filename), writeStream(file, symbolMask, symbolTable, delimiter) {

    }
    virtual void writeNextTuple(const RamDomain *tuple) {
        writeStream.writeNextTuple(tuple);
    }

    virtual ~WriteFileCSV() {}
private:
    std::ofstream file;
    WriteStreamCSV writeStream;
};

class WriteCoutCSV: public WriteStream {
public:
    WriteCoutCSV(const std::string& relationName, const SymbolMask& symbolMask, const SymbolTable &symbolTable,
            char delimiter = '\t') :
                writeStream(std::cout, symbolMask, symbolTable, delimiter) {
        std::cout << "---------------\n" << relationName << "\n===============\n";
    }
    virtual void writeNextTuple(const RamDomain *tuple) {
        writeStream.writeNextTuple(tuple);
    }

    virtual ~WriteCoutCSV() {
        std::cout << "===============\n";
    }
private:
    WriteStreamCSV writeStream;
};

} /* namespace souffle */
