#pragma once

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include "Tui.hpp"



class Cli {
public:

    std::vector<std::string> args;

    Cli(int argc, char *argv[]) :
            args()
    {
        for (int i=0; i<argc; i++) {
            args.push_back(std::string(argv[i]));
        }
    }

	void error();

	void parse();
};
