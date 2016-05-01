/*
 * Souffle version 0.0.0
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - souffle/LICENSE
 */

/************************************************************************
 *
 * @file RamRecords.h
 *
 * Utilities for handling records in the interpreter
 *
 ***********************************************************************/

#pragma once

#include "RamTypes.h"

namespace souffle {

/**
 * A function packing a tuple of the given arity into a reference.
 */
RamDomain pack(RamDomain* tuple, int arity);

/**
 * A function obtaining a pointer to the tuple addressed by the given reference.
 */
RamDomain* unpack(RamDomain ref, int arity);

/**
 * Obtains the null-reference constant.
 */
RamDomain getNull();

/**
 * Determines whether the given reference is the null reference encoding
 * the absence of any nested record.
 */
bool isNull(RamDomain ref);

} // end of namespace souffle

