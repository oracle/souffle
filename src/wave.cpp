/*
 * Souffle-Wave - Souffle's pre-processor
 * Copyright (c) 2016 by the Souffle Team.
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

#include "Util.h"
//#include "wave.hpp"                                    // global configuration

///////////////////////////////////////////////////////////////////////////////
// Include additional Boost libraries
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/convenience.hpp>
#include <boost/timer.hpp>
#include <boost/any.hpp>

///////////////////////////////////////////////////////////////////////////////
//  Include Wave itself
#include <wavelib/wave.hpp>

///////////////////////////////////////////////////////////////////////////////
//  Include the lexer related stuff
#include <wavelib/cpplexer/cpp_lex_token.hpp>      // token type
#include <wavelib/cpplexer/cpp_lex_iterator.hpp>   // lexer type

///////////////////////////////////////////////////////////////////////////////
//  Include the context policies to use
#include "wave_trace_macro_expansion.hpp"

///////////////////////////////////////////////////////////////////////////////
#include <wavelib/cpplexer/re2clex/cpp_re2c_lexer.hpp>

///////////////////////////////////////////////////////////////////////////////
//  Include the grammar definitions, if these shouldn't be compiled separately
//  (ATTENTION: _very_ large compilation times!)
#include <wavelib/grammars/cpp_intlit_grammar.hpp>
#include <wavelib/grammars/cpp_chlit_grammar.hpp>
#include <wavelib/grammars/cpp_grammar.hpp>
#include <wavelib/grammars/cpp_expression_grammar.hpp>
#include <wavelib/grammars/cpp_predef_macros_grammar.hpp>
#include <wavelib/grammars/cpp_defined_grammar.hpp>

///////////////////////////////////////////////////////////////////////////////
//  Import required names
using namespace boost::spirit::classic;

using std::pair;
using std::vector;
using std::getline;
using std::ofstream;
using std::cout;
using std::cerr;
using std::endl;
using std::ostream;
using std::istreambuf_iterator;

///////////////////////////////////////////////////////////////////////////////
//
//  This application uses the lex_iterator and lex_token types predefined
//  with the Wave library, but it is possible to use your own types.
//
//  You may want to have a look at the other samples to see how this is
//  possible to achieve.
typedef boost::wave::cpplexer::lex_token<> token_type;
typedef boost::wave::cpplexer::lex_iterator<token_type> lex_iterator_type;

//  The C++ preprocessor iterators shouldn't be constructed directly. They
//  are to be generated through a boost::wave::context<> object. This
//  boost::wave::context object is additionally to be used to initialize and
//  define different parameters of the actual preprocessing.
typedef boost::wave::context<std::string::iterator, lex_iterator_type,
    boost::wave::iteration_context_policies::load_file_to_string,
    trace_macro_expansion<token_type> > context_type;

//////////////////////////////////////////////////////////////////////////
//  Generate some meaningful error messages
template <typename Exception>
inline int report_error_message(Exception const &e)
{
    // default error reporting
    cerr << e.file_name() << ":" << e.line_no() << ":" << e.column_no()
         << ": " << e.description() << endl;

    // errors count as one
    return (e.get_severity() == boost::wave::util::severity_error ||
            e.get_severity() == boost::wave::util::severity_fatal) ? 1 : 0;
}

///////////////////////////////////////////////////////////////////////////
// read all of a file into a string
std::string read_entire_file(std::istream& instream)
{
    std::string content;

    instream.unsetf(std::ios::skipws);

    content = std::string(std::istreambuf_iterator<char>(instream.rdbuf()),
                          std::istreambuf_iterator<char>());
    return content;
}

///////////////////////////////////////////////////////////////////////////////
//  do the actual preprocessing
int process (std::string file_name, 
             std::istream &instream, 
             const std::vector<std::string> &include_paths) 
{
    // current file position is saved for exception handling
    boost::wave::util::file_position_type current_position;
    int error_count = 0;

    try {
        // process the given file
        std::string instring;
        instream.unsetf(std::ios::skipws);
        instring = read_entire_file(instream);
           
 

        std::ofstream output, tracestream, includestream, guardstream;
        bool generate_output; 
        std::string default_out;  

        // This this the central piece of the Wave library, it provides you with
        // the iterators to get the preprocessed tokens and allows to configure
        // the preprocessing stage in advance.
        trace_macro_expansion<token_type> hooks(
            true, // preserve whitespace
            true, // preserve bol whitespace
            output, 
            tracestream, 
            includestream, 
            guardstream, 
            trace_nothing,
            false,  // enable system command
            generate_output,
            default_out);


        // The preprocessing of the input stream is done on the fly behind the
        // scenes during iteration over the context_type::iterator_type stream.
        context_type ctx (instring.begin(), 
                instring.end(), 
                file_name.c_str(),  
                hooks);

        ctx.set_language(boost::wave::enable_preserve_comments(ctx.get_language()));
        ctx.set_language( boost::wave::enable_emit_line_directives(ctx.get_language(), true));

        for (auto cur : include_paths) { 
            ctx.add_include_path(cur.c_str());
        }

        // analyze the input file
        context_type::iterator_type first = ctx.begin();
        context_type::iterator_type last = ctx.end();

        std::cout << "#line 1 \"" << file_name << "\"\n";

        bool finished = false;
        bool advance = false; 
        do {
            try {
                if (advance) {
                    ++first;
                    advance = false;
                } 
                while (first != last) {
                    // store the last known good token position
                    current_position = (*first).get_position();

                    // print out the current token value
                    cout << (*first).get_value();

                    // advance to the next token
                    ++first;
                }
                finished = true;
            } catch (boost::wave::cpp_exception const &e) {
                if (boost::wave::is_recoverable(e) && !(e.get_severity() == boost::wave::util::severity_error )) {
                    advance = true;   // advance to the next token
                } else {
                    throw;      // re-throw for non-recoverable errors
                }
            } catch (boost::wave::cpplexer::lexing_exception const &e) {
                // some preprocessing error
                if ( boost::wave::cpplexer::is_recoverable(e)) {
                    advance = true;   // advance to the next token
                } else {
                    throw;      // re-throw for non-recoverable errors
                }
            }
        } while (!finished);
    } catch (boost::wave::cpp_exception const &e) {
        // some preprocessing error
        report_error_message(e);
        return 1;
    } catch (boost::wave::cpplexer::lexing_exception const &e) {
        // some lexing error
        report_error_message(e);
        return 2;
    } catch (std::exception const &e) {
        // use last recognized token to retrieve the error position
        cerr << current_position << ": "
             << "exception caught: " << e.what()
             << endl;
        return 3;
    } catch (...) {
        // use last recognized token to retrieve the error position
        cerr << current_position << ": "
             << "unexpected exception caught." << endl;
        return 4;
    } return -error_count;  // returns the number of errors as a negative integer
}

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
    std::cerr << "souffle-wave -- souffle's pre-processor\n";
    std::cerr << "Usage: souffle [OPTION] FILE.\n";
    std::cerr << "-------------------------------------------------------------------------------------------------------\n";
    std::cerr << "Options:\n";
    std::cerr << "    -I<DIR>, --include-dir=<DIR>   Specify directory for include files\n";
    std::cerr << "\n";
    std::cerr << "    -v, --verbose                  Verbose output\n";
    std::cerr << "-------------------------------------------------------------------------------------------------------\n";
    std::cerr << "Version: " << PACKAGE_VERSION << "\n";
    std::cerr << "-------------------------------------------------------------------------------------------------------\n";
    std::cerr << "Copyright (c) 2001-2012 Hartmut Kaiser. Distributed under the Boost\n";
    std::cerr << "Copyright (c) 2016 by the Souffle Team\n";
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

int main(int argc, char **argv)
{

    std::vector<std::string> includes; /* include options for c-preprocessor */
    bool verbose  = false;             /* flag for verbose output */

    // long options
    option longOptions[] = {
        { "include-dir", true, nullptr, 'I' },
        //
        { "verbose", false, nullptr, 'v' },
        // the terminal option -- needs to be null
        { nullptr, false, nullptr, 0 }
    };

    int c;     /* command-line arguments processing */
    while ((c = getopt_long(argc, argv, "I:v", longOptions, nullptr)) != EOF) {
        switch(c) {
            /* Print debug / profiling information */
            case 'v':
                verbose = true;
                break;

                /* Include directory for Datalog specifications */
            case 'I':
                if (!souffle::existDir(optarg)) {
                    fail("error: include directory " + std::string(optarg) + " does not exists");
                }
                includes.push_back(optarg); 
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

    /* collect all input files for the C pre-processor */
    std::string filename;
    if (optind + 1 == argc ) {
        if (!souffle::existFile(argv[optind])) {
            fail("error: cannot open file " + std::string(argv[optind]));
        }
        filename = argv[optind];
    } else {
        helpPage(true,argc,argv);
        return 0; 
    }
    std::ifstream instream(filename.c_str());

    return process(filename, instream, includes) ;
}
