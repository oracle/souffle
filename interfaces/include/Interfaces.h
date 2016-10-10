/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file Interfaces.h
 *
 * Interfaces for Souffle
 *
 ***********************************************************************/
#pragma once

#include <stdlib.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <getopt.h>
#include <config.h>

#include <chrono>
#include <list>
#include <iostream>
#include <fstream>
#include <string>

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <assert.h>
#include <initializer_list>

#include "AstProgram.h"
#include "AstUtils.h"
#include "AstTuner.h"
#include "AstAnalysis.h"
#include "AstTranslationUnit.h"
#include "AstTransformer.h"
#include "AstSemanticChecker.h"
#include "AstTransforms.h"
#include "PrecedenceGraph.h"
#include "Util.h"
#include "SymbolTable.h"
#include "ParserDriver.h"
#include "AstBuilder.h"
#include "ComponentModel.h"

#include "RamTranslator.h"
#include "RamExecutor.h"
#include "RamStatement.h"

#include "RamTypes.h"
#include "SymbolTable.h"
#include "Executor.h"

#include "Logger.h"

namespace souffle {

struct Flags {
    std::string debugReportFile = ""; /* filename to output debug report */
    std::string outputDir = "."; /* output directory for resulting csv files */
    std::string includeOpt = "";      /* include options for c-preprocessor */
    std::string profile = "./tiros.log";         /* filename of profile log */
    std::string outputFileName = "";
    std::string factFileDir = ".";
    std::string outputHeaderFileName = "";

    bool nowarn  = true;        /* flag for verbose output */
    bool verbose  = false;        /* flag for verbose output */
    bool compile = false;         /* flag for enabling compilation */
    bool tune = false;            /* flag for enabling / disabling the rule scheduler */
    bool logging = true;         /* flag for profiling */
    bool debug = false;           /* flag for enabling debug mode */
    bool generateHeader = false;  /* flag for enabling code generation mode */

    enum {optAutoSchedule=1,
          optDebugReportFile=2} Opt;

    /* collect all input files for the C pre-processor */
    std::string filenames = "";
    std::string programName = "";
    int num_threads = 8;
};


class InternalInterface {
public:
    InternalInterface(Flags& flags) : 
      flags(flags),
      exec(NULL)
      {}

    ~InternalInterface(){ delete exec; }

    Executor* parse(AstBuilder* driver);
    InterfaceResult* executeInterpreter(RamData* data);
    InterfaceResult* executeCompiler(RamData* data, std::string& filename);
    
protected:
    Flags flags;
    Executor* exec;
};

}
