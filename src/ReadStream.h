/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */


/************************************************************************
 *
 * @file ReadStream.h
 *
 ***********************************************************************/

#pragma once

#include <memory>

#include "RamTypes.h"

namespace souffle {

class ReadStream {
public:
    virtual std::unique_ptr<RamDomain[]> readNextTuple() = 0;
    virtual ~ReadStream() {};
};

} /* namespace souffle */
