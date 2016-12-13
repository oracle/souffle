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

namespace souffle {

class WriteStream {
public:
  virtual void writeNextTuple(const RamDomain &tuple) = 0;
  virtual ~WriteStream() {};
};

} /* namespace souffle */
