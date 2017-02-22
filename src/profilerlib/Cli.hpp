/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

#pragma once

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "Tui.hpp"

/*
 * CLI to parse command line arguments and start up the TUI to either run a single command,
 * generate the GUI file or run the TUI
 */
class Cli {

public:
    std::vector<std::string> args;

    Cli(int argc, char* argv[]) : args() {
        for (int i = 0; i < argc; i++) {
            args.push_back(std::string(argv[i]));
        }
    }

    void error();

    void parse();
};
