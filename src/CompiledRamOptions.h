/*
 * Copyright (c) 2013-15, Oracle and/or its affiliates.
 *
 * All rights reserved.
 */

/************************************************************************
 *
 * @file CompiledRamOptions.h
 *
 * A header file offering command-line option support for compiled
 * RAM programs.
 *
 ***********************************************************************/

#pragma once

#include <string>
#include <iostream>
#include <getopt.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#ifdef _OPENMP
    #include <omp.h>
#endif

/**
 * A utility class for parsing command line arguments within generated
 * query programs.
 */
struct CmdOptions {

    /**
     * The name of the source file this analysis has been compiled from.
     */
    std::string analysis_src;

    /**
     * The name of the directory to read facts from.
     */
    std::string input_dir;

    /**
     * The name of the directory to store output relations.
     */
    std::string output_dir;

    /**
     * The name of the profile filename 
     */
    std::string profile_fname; 

    /**
     * Flag indicating whether profiling is switched on 
     */ 
    bool profiling_enabled; 

    /**
     * Flag indicating whether debug mode is enabled
     */
    bool debug_enabled;

    // default constructor
    CmdOptions(bool pe = false, bool de = false)
        : analysis_src("-unknown-"),
          input_dir("."),
          output_dir("."), 
          profile_fname(""), 
          profiling_enabled(pe),
          debug_enabled(de) {}

    /**
     * Parses the given command line parameters, handles -h help requests or errors
     * and returns whether the parsing was successful or not.
     */
    bool parse(int argc, char **argv) {

        // get executable name
        std::string exec_name = "analysis";
        if (argc > 0) exec_name = argv[0];

        // local options
        std::string fact_dir = input_dir;
        std::string out_dir = output_dir;

// avoid warning due to Solaris getopt.h
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wwrite-strings"
        // long options
        option longOptions[] = {
            { "facts", true, nullptr, 'F' },
            { "output", true, nullptr, 'D' },
            { "profile", true, nullptr, 'p' },
#ifdef _OPENMP
            { "jobs", true, nullptr, 'j' },
#endif
            // the terminal option -- needs to be null
            { nullptr, false, nullptr, 0 }
        };
#pragma GCC diagnostic pop

        // check whether all options are fine
        bool ok = true;

        int c;     /* command-line arguments processing */
        while ((c = getopt_long(argc, argv, "D:F:hp:j:o:", longOptions, nullptr)) != EOF) {
            switch(c) {
                    /* Fact directories */
                case 'F':
                    if (!existDir(optarg)) {
                        printf("Fact directory %s does not exists!\n", optarg);
                        ok = false;
                    }
                    fact_dir = optarg;
                    break;
                    /* Output directory for resulting .csv files */
                case 'D':
                    if (*optarg && !existDir(optarg)) {
                        printf("Output directory %s does not exists!\n", optarg);
                        ok = false;
                    }
                    out_dir = optarg;
                    break;
                case 'p':
                    if (!profiling_enabled) {
                        std::cerr << "\nerror: profiling was not enabled in compilation\n\n";
                        printHelpPage(exec_name);
                        exit(1);
                    }
                    profile_fname = optarg;
                    break;
#ifdef _OPENMP
                case 'j': {
                    int num = atoi(optarg);
                    if (num > 0) {
                        omp_set_num_threads(num);
                    } else {
                        std::cerr << "Invalid number of jobs [-j]: " << optarg << "\n";
                        ok = false;
                    }
                    break;
                }
#else
                case 'j':
#endif
                case 'h':
                case '?':
                    printHelpPage(exec_name);
                    return false;
                default:
                    ASSERT("Default label in getopt switch");
            }
        }

        // update member fields
        input_dir = fact_dir;
        output_dir = out_dir;

        // return success state
        return ok;
    }

private:

    /**
     * Prints the help page in case it has been requested or there was a typer in the command line arguments.
     */
    void printHelpPage(const std::string& exec_name) const {

        std::cerr << "====================================================================\n";
        std::cerr << " Datalog Program: " << analysis_src << "\n";
        std::cerr << " Usage: " << exec_name << " [OPTION]\n\n";
        std::cerr << " Options:\n";
        std::cerr << "    -D <DIR>, --output=<DIR>     -- directory for output relations\n";
        std::cerr << "                                    (default: " << output_dir << ")\n";
        std::cerr << "                                    (suppress output with \"\")\n";
        std::cerr << "    -F <DIR>, --facts=<DIR>      -- directory for fact files\n";
        std::cerr << "                                    (default: " << input_dir << ")\n";
        if (profiling_enabled) {
            std::cerr << "    -p <file>, --profile=<file>  -- Specify filename for profile, default: " << profile_fname << "\n";
        }
#ifdef _OPENMP
        std::cerr << "    -j <NUM>, --jobs=<NUM>       -- number of threads to use for parallel processing.\n";
#endif
        std::cerr << "    -h                           -- prints this help page.\n";
        std::cerr << "--------------------------------------------------------------------\n";
        std::cerr << " Copyright (c) 2013-15, Oracle and/or its affiliates.\n";
        std::cerr << " All rights reserved.\n";
        std::cerr << "====================================================================\n";

    }

    /**
     *  Check whether a file exists in the file system
     */
    inline bool existFile (const std::string& name) {
        struct stat buffer;
        if (stat (name.c_str(), &buffer) == 0) {
            if ((buffer.st_mode & S_IFREG) != 0) {
                return true;
            }
        }
        return false;
    }

    /**
     *  Check whether a directory exists in the file system
     */
    bool existDir (const std::string& name) {
        struct stat buffer;
        if (stat (name.c_str(), &buffer) == 0) {
            if ((buffer.st_mode & S_IFDIR) != 0) {
                return true;
            }
        }
        return false;
    }
};
