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

class WriteStreamCSV : public WriteStream {
public:
    WriteStreamCSV(const std::string &fname, const SymbolMask& format, SymbolTable &symtab, char delimiter = '\t');
    virtual void writeNextTuple(const RamDomain &tuple);
    virtual ~WriteStreamCSV();
};

} /* namespace souffle */
