/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file AstTuner.h
 *
 * Provides utilities to conduct automated tuning of AstStructures.
 *
 ***********************************************************************/

#pragma once

#include "RamExecutor.h"
#include "AstTransformer.h"

namespace souffle {

class AstTranslationUnit;

/**
 * Transformation pass which tunes the given program by re-ordering the atoms
 * in the included rules to improve performance.
 *
 * This operation is based on an actual, interpreted execution of the
 * given program and the collection of profiling data.
 */
class AutoScheduleTransformer : public AstTransformer {
private:
    std::string factDir;
    bool verbose;
    bool generateScheduleReport;

    virtual bool transform(AstTranslationUnit &translationUnit);

public:
    AutoScheduleTransformer(const std::string &factDir, bool verbose = false, bool generateScheduleReport = false) :
        factDir(factDir), verbose(verbose), generateScheduleReport(generateScheduleReport) { }

    virtual ~AutoScheduleTransformer() { }

    virtual std::string getName() const {
        return "AutoScheduleTransformer";
    }

    /**
     * Perform auto-scheduling for the given program.
     * @return whether the program was modified
     */
    static bool autotune(AstTranslationUnit& translationUnit, const std::string& factDir, std::ostream* report, bool verbose = false, const QueryExecutionStrategy& strategy = ScheduledExecution);
};

} // end of namespace souffle

