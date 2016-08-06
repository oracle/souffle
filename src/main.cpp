/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file main.cpp
 *
 * Main driver for Souffle
 *
 ***********************************************************************/

#include <config.h>
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

#include <chrono>
#include <list>
#include <iostream>
#include <fstream>
#include <string>

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
#include "ComponentModel.h"

#include "RamTranslator.h"
#include "RamExecutor.h"
#include "RamStatement.h"

namespace souffle {


/**
 *  Show help page.
 */
void helpPage(bool error, int argc, char**argv)
{
    if (error) {
        for(int i=0;i<argc;i++) {
            std::cerr << argv[i] << " ";
        }
        std::cerr << "\nError parsing command-line arguments\n";
    }
    std::cerr << "=======================================================================================================\n";
    std::cerr << "souffle -- Oracle Labs' datalog engine.\n";
    std::cerr << "Usage: souffle [OPTION] FILE.\n";
    std::cerr << "-------------------------------------------------------------------------------------------------------\n";
    std::cerr << "Options:\n";
    std::cerr << "    -F<DIR>, --fact-dir=<DIR>      Specify directory for fact files\n";
    std::cerr << "    -I<DIR>, --include-dir=<DIR>   Specify directory for include files\n";
    std::cerr << "    -D<DIR>, --output-dir=<DIR>    Specify directory for output relations\n";
    std::cerr << "                                      (if <DIR> is -, output is written to stdout)\n";
    std::cerr << "\n";
    std::cerr << "    -j<N>, --jobs=<N>              Run interpreter/compiler in parallel using N threads, N=auto for system default\n";
    std::cerr << "\n";
    std::cerr << "    -c, --compile                  Compile datalog (translating to C++)\n";
    std::cerr << "    --auto-schedule                Switch on automated clause scheduling for compiler\n";
    std::cerr << "    -g <FILE>                      Only generate sources of compilable analysis and write it to <FILE>\n";
    std::cerr << "    -w                             Disable warnings\n";
    std::cerr << "    -o <FILE>, --dl-program=<FILE> Write executable program to <FILE> (without executing it)\n";
    std::cerr << "\n";
    std::cerr << "    -p<FILE>, --profile=<FILE>     Enable profiling and write profile data to <FILE>\n";
    std::cerr << "    -d, --debug                    Enable debug mode\n";
    std::cerr << "\n";
    std::cerr << "    --debug-report=<FILE>          Write debugging output to HTML report\n";
    std::cerr << "\n";
    std::cerr << "    -v, --verbose                  Verbose output\n";
    std::cerr << "-------------------------------------------------------------------------------------------------------\n";
    std::cerr << "Version: " << PACKAGE_VERSION << "\n";
    std::cerr << "-------------------------------------------------------------------------------------------------------\n";
    std::cerr << "Copyright (c) 2013, 2015, Oracle and/or its affiliates.\n";
    std::cerr << "All rights reserved.\n";
    std::cerr << "=======================================================================================================\n";
    exit(1);
}

/**
 *  print error message and exit
 */
void fail(const std::string &str)
{
    std::cerr << str << "\n";
    exit(1);
}



static void wrapPassesForDebugReporting(std::vector<std::unique_ptr<AstTransformer>> &transforms) {
    for (unsigned int i = 0; i < transforms.size(); i++) {
        transforms[i] = std::unique_ptr<AstTransformer>(new DebugReporter(std::move(transforms[i])));
    }
}

int main(int argc, char **argv)
{
    /* Time taking for overall runtime */
    auto souffle_start = std::chrono::high_resolution_clock::now();

    std::string debugReportFile; /* filename to output debug report */
    std::string outputDir = "."; /* output directory for resulting csv files */
    std::string includeOpt;      /* include options for c-preprocessor */
    std::string profile;         /* filename of profile log */

    std::string outputFileName = "";
    std::string factFileDir = ".";

    std::string outputHeaderFileName = "";

    bool nowarn  = false;        /* flag for disabled warnings */
    bool verbose  = false;        /* flag for verbose output */
    bool compile = false;         /* flag for enabling compilation */
    bool tune = false;            /* flag for enabling / disabling the rule scheduler */
    bool logging = false;         /* flag for profiling */
    bool debug = false;           /* flag for enabling debug mode */
    bool generateHeader = false;  /* flag for enabling code generation mode */

    enum {optAutoSchedule=1,
          optDebugReportFile=2};

    // long options
    option longOptions[] = {
        { "fact-dir", true, nullptr, 'F' },
        { "include-dir", true, nullptr, 'I' },
        { "output-dir", true, nullptr, 'D' },
        //
        { "jobs", true, nullptr, 'j' },
        //
        { "compile", false, nullptr, 'c' },
        { "nowarn", false, nullptr, 'w' },
        { "auto-schedule", false, nullptr, optAutoSchedule },
        { "dl-program", true, nullptr, 'o' },
        //
        { "debug-report", true, nullptr, optDebugReportFile },
        //
        { "profile", true, nullptr, 'p' },
        //
        { "debug", false, nullptr, 'd' },
        //
        { "verbose", false, nullptr, 'v' },
        // the terminal option -- needs to be null
        { nullptr, false, nullptr, 0 }
    };

    static unsigned num_threads = 1;	  /* the number of threads to use for execution, 0 system-choice, 1 sequential */
    int c;     /* command-line arguments processing */
    while ((c = getopt_long(argc, argv, "cj:D:F:I:p:o:g:hvwd", longOptions, nullptr)) != EOF) {
        switch(c) {
                /* Print debug / profiling information */
            case 'v':
                verbose = true;
                break;

                /* Enable compiler flag */
            case 'c':
                compile = true;
                break;
            case 'w':
                nowarn = true;
                break;
                /* Set number of jobs */
            case 'j':
                if (std::string(optarg) == "auto") {
                   num_threads = 0;
                } else if(isNumber(optarg)) {
                   num_threads = atoi(optarg);
                   if(num_threads == 0) {
                       fail("Number of jobs in the -j/--jobs options must be greater than zero!");
                   }
                } else fail("Wrong parameter "+std::string(optarg)+" for option -j/--jobs!");
                break;

                /* Output file name of generated executable program */
            case 'o':
                outputFileName = optarg;
                compile = true;
                break;

                /* Generator mode and output file name */
            case 'g':
            	outputHeaderFileName = optarg;
            	generateHeader = true;
            	break;

                /* Fact directories */
            case 'F':
                // Add conditional check:
                // if (factFileDir != NULL) {
                //   fail("Fact directory option can only be specified once (-F%s -F%s)!\n", factFileDir, optarg);
                // } else if (!existDir(optarg)) {
                //   fail("Fact directory %s does not exists!\n", optarg);
                //}
                factFileDir = optarg;
                break;

                /* Output directory for resulting .csv files */
            case 'D':
                outputDir = optarg;
                if (strcmp(optarg, "-") && !existDir(optarg)) {
                    fail("error: ouput directory " + std::string(optarg) + " does not exists");
                }
                break;

                /* Include directory for Datalog specifications */
            case 'I':
                if (!existDir(optarg)) {
                    fail("error: include directory " + std::string(optarg) + " does not exists");
                }
                if(includeOpt == "") {
                    includeOpt = "-I" + std::string(optarg);
                } else {
                    includeOpt = includeOpt + " -I" + std::string(optarg);
                }
                break;

                /* File-name for profile log */
            case 'p':
                logging = true;
                profile = optarg;
                break;

                /* Enable debug mode */
            case 'd':
                debug = true;
                break;

                /* Enable auto scheduler */
            case optAutoSchedule:
                tune = true;
                compile = true;
                break;

                /* Enable generation of debug output report */
            case optDebugReportFile:
                debugReportFile = optarg;
                break;

                /* Show help page */
            case 'h':
                helpPage(false,argc,argv);
                break;

            case '?':
                helpPage(true,argc,argv);
                break;

            default:
                ASSERT("Default label in getopt switch");
        }
    }

    if (tune && outputFileName == "") {
       fail("error: no executable is specified for auto-scheduling (option -o <FILE>)");
    }


    /* collect all input files for the C pre-processor */
    std::string filenames = "";
    if (optind < argc) {
        for (; optind < argc; optind++) {
            if (!existFile(argv[optind])) {
                fail("error: cannot open file " + std::string(argv[optind]));
            }
            if (filenames == "") {
                filenames = argv[optind];
            } else {
                filenames = filenames + " " + std::string(argv[optind]);
            }
        }
    } else {
        helpPage(true,argc,argv);
    }

    std::string programName = which(argv[0]);
    if (programName.empty())
        fail("error: failed to determine souffle executable path");

    /* Create the pipe to establish a communication between cpp and souffle */
    std::string cmd = ::findTool("souffle-wave", programName, ".");

    if (!isExecutable(cmd))
        fail("error: failed to locate souffle preprocessor");

    cmd  += " " + includeOpt + " " + filenames;
    FILE* in = popen(cmd.c_str(), "r");

    /* Time taking for parsing */
    auto parser_start = std::chrono::high_resolution_clock::now();

    // ------- parse program -------------

    // parse file
    std::unique_ptr<AstTranslationUnit> translationUnit = ParserDriver::parseTranslationUnit("<stdin>", in, nowarn);

    // close input pipe
    int preprocessor_status = pclose(in);
    if (preprocessor_status == -1) {
        perror(NULL);
        fail("error: failed to close pre-processor pipe");
    }

    /* Report run-time of the parser if verbose flag is set */
    if (verbose) {
        auto parser_end = std::chrono::high_resolution_clock::now();
        std::cout << "Parse Time: " << std::chrono::duration<double>(parser_end-parser_start).count()<< "sec\n";
    }

    // ------- check for parse errors -------------
    if (translationUnit->getErrorReport().getNumErrors() != 0) {
        std::cerr << translationUnit->getErrorReport();
        fail(std::to_string(translationUnit->getErrorReport().getNumErrors()) + " errors generated, evaluation aborted");
    }

    // ------- rewriting / optimizations -------------

    std::vector<std::unique_ptr<AstTransformer>> transforms;
    transforms.push_back(std::unique_ptr<AstTransformer>(new ComponentInstantiationTransformer()));
    transforms.push_back(std::unique_ptr<AstTransformer>(new UniqueAggregationVariablesTransformer()));
    transforms.push_back(std::unique_ptr<AstTransformer>(new AstSemanticChecker()));
    transforms.push_back(std::unique_ptr<AstTransformer>(new ResolveAliasesTransformer()));
    transforms.push_back(std::unique_ptr<AstTransformer>(new RemoveRelationCopiesTransformer()));
    transforms.push_back(std::unique_ptr<AstTransformer>(new MaterializeAggregationQueriesTransformer()));
    transforms.push_back(std::unique_ptr<AstTransformer>(new RemoveEmptyRelationsTransformer()));
    if (!debug) {
        transforms.push_back(std::unique_ptr<AstTransformer>(new RemoveRedundantRelationsTransformer()));
    }
    transforms.push_back(std::unique_ptr<AstTransformer>(new AstExecutionPlanChecker()));
    if (tune) {
        transforms.push_back(std::unique_ptr<AstTransformer>(new AutoScheduleTransformer(factFileDir, verbose, !debugReportFile.empty())));
    }
    if (!debugReportFile.empty()) {
        auto parser_end = std::chrono::high_resolution_clock::now();
        std::string runtimeStr = "(" + std::to_string(std::chrono::duration<double>(parser_end-parser_start).count()) + "s)";
        DebugReporter::generateDebugReport(*translationUnit, "Parsing", "After Parsing " + runtimeStr);
        wrapPassesForDebugReporting(transforms);
    }

    for (const auto &transform : transforms) {
        transform->apply(*translationUnit);

        /* Abort evaluation of the program if errors were encountered */
        if (translationUnit->getErrorReport().getNumErrors() != 0) {
            std::cerr << translationUnit->getErrorReport();
            fail(std::to_string(translationUnit->getErrorReport().getNumErrors()) + " errors generated, evaluation aborted");
        }
    }
    if (translationUnit->getErrorReport().getNumIssues() != 0) {
        std::cerr << translationUnit->getErrorReport();
    }


    // ------- execution -------------

    auto ram_start = std::chrono::high_resolution_clock::now();

    /* translate AST to RAM */
    std::unique_ptr<RamStatement> ramProg = RamTranslator(logging).translateProgram(*translationUnit);

    if (!debugReportFile.empty()) {
        if (ramProg) {
            auto ram_end = std::chrono::high_resolution_clock::now();
            std::string runtimeStr = "(" + std::to_string(std::chrono::duration<double>(ram_end-ram_start).count()) + "s)";
            std::stringstream ramProgStr;
            ramProgStr << *ramProg;
            translationUnit->getDebugReport().addSection(DebugReporter::getCodeSection("ram-program", "RAM Program " + runtimeStr, ramProgStr.str()));
        }

        if (!translationUnit->getDebugReport().empty()) {
            std::ofstream debugReportStream(debugReportFile);
            debugReportStream << translationUnit->getDebugReport();
        }
    }

    /* run RAM program */
    if (!ramProg) return 0;

    // pick executor

    std::unique_ptr<RamExecutor> executor;
    if (generateHeader || compile) {
        // configure compiler
        executor = std::unique_ptr<RamExecutor>(new RamCompiler(outputFileName));
        if (verbose) {
           executor -> setReportTarget(std::cout);
        }
    } else {
        // configure interpreter
        if (tune) {
            executor = std::unique_ptr<RamExecutor>(new RamGuidedInterpreter());
        } else {
            executor = std::unique_ptr<RamExecutor>(new RamInterpreter());
        }
    }

    // configure executor
    auto& config = executor->getConfig();
    config.setSourceFileName(filenames);
    config.setFactFileDir(factFileDir);
    config.setOutputDir(outputDir);
    config.setNumThreads(num_threads);
    config.setLogging(logging);
    config.setProfileName(profile);
    config.setDebug(debug);

    /* Locate souffle-compile script */
    std::string compileCmd = ::findTool("souffle-compile", programName, ".");

    /* Fail if a souffle-compile executable is not found */
    if (!isExecutable(compileCmd))
        fail("error: failed to locate souffle-compile");

    config.setCompileScript(compileCmd + " ");

    // check if this is code generation only
    if (generateHeader) {

    	// just generate, no compile, no execute
		static_cast<const RamCompiler*>(executor.get())->generateCode(translationUnit->getSymbolTable(), *ramProg, outputHeaderFileName);

    	// check if this is a compile only
    } else if (compile && outputFileName != "") {
        // just compile, no execute
        static_cast<const RamCompiler*>(executor.get())->compileToBinary(translationUnit->getSymbolTable(), *ramProg);
    } else {
        // run executor
        executor->execute(translationUnit->getSymbolTable(), *ramProg);
    }

    /* Report overall run-time in verbose mode */
    if (verbose) {
        auto souffle_end = std::chrono::high_resolution_clock::now();
        std::cout << "Total Time: " << std::chrono::duration<double>(souffle_end-souffle_start).count() << "sec\n";
    }

    return 0;
}

} // end of namespace souffle

int main(int argc, char **argv) {
    return souffle::main(argc, argv);
}


