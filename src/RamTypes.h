/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file RamTypes.h
 *
 * Defines tuple element type and data type for keys on table columns
 *
 ***********************************************************************/

#pragma once

#include <limits>

#include <stdint.h>

namespace souffle {

/**
 * Type of an element in a tuple.
 *
 * Default type is int32_t; may be overridden by user
 * defining RAM_DOMAIN_TYPE.
 */
#ifdef RAM_DOMAIN_TYPE
typedef RAM_DOMAIN_TYPE RamDomain;
#else
typedef int32_t RamDomain;
#endif

/** lower and upper boundaries for the ram domain **/
#define MIN_RAM_DOMAIN (std::numeric_limits<RamDomain>::min())
#define MAX_RAM_DOMAIN (std::numeric_limits<RamDomain>::max())

/** type of an index key; each bit represents a column of a table */
typedef uint64_t SearchColumns;

}  // end of namespace souffle
