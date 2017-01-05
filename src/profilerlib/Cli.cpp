/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/


#include "Cli.hpp"





void Cli::error() {
    //std::cout << "\nExpected commands/args to run souffle-prof: \n";
    std::cout << "./souffle-profile [-f|-j <file> [-c <command>] [-l]] [-h]\n";
    exit(1);
}

void Cli::parse() {
    std::vector<std::string> commands;
    std::string filename;
    bool alive = false;

    if (args.size() <= 1) {
        error();
    }

    for (size_t i=1; i<args.size(); i++) {
        std::string arg = args.at(i++);
        if (arg.at(0) != '-') {
            error();
        }

        if (arg.compare("-h")==0) {
            std::cout << "Souffle Profiler Alpha 4 (14/12/16)\n";
            error();
        } else if (arg.compare("-c")==0) {
            if (i < args.size()) {
                // split at \s+ and save into commands
                std::string& command_str = args.at(i++);
                commands = Tools::split(command_str, " ");

            } else {
                std::cout << "Parameters for option -c missing!\n";
                error();
            }
        } else if (arg.compare("-f")==0) {
            if (i < args.size()) {
                filename = args.at(i);
            } else {
                std::cout << "Parameters for option -f missing!\n";
                error();
            }
        } else if (arg.compare("-j")==0) {
            if (i < args.size()) {
                filename = args.at(i);
            } else {
                std::cout << "Parameters for option -j missing!\n";
                error();
            }
        } else if (arg.compare("-l")==0) {
            alive = true;
        } else {
            std::cout << "Unknown argument " << args.at(i) << "\n";
            error();
        }
    }



    if (commands.size() > 0) {
        Tui(filename, alive).runCommand(commands);
    } else {
        if (args.at(1).compare("-j") == 0) {
            Tui(filename, alive).outputJson();
        } else {
            Tui(filename, alive).runProf();
        }
    }
}
