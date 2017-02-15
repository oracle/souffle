/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

#include "profilerlib/Cli.hpp"

#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    Cli cli_obj = Cli(argc, argv);
    cli_obj.parse();
    std::cout << "\n";
    return 0;
}
