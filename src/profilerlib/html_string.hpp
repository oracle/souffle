/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include "StringUtils.hpp"

#define gui_directory = "gui_src"

class html_string {
private:
    std::string first_half;
    std::string second_half;
public:
    html_string() {
        std::string current_dir = __FILE__;
        std::cout << "__FILE__ => " << current_dir << std::endl;
        std::cout << "__BASE_FILE__ => " << __BASE_FILE__ << std::endl;
        std::cout << "MAKEDIR => " << MAKEDIR << std::endl;
        throw("memes");
    }

    inline std::string get_first_half() {

        return this->first_half;
    }

    inline std::string get_second_half() {
        return this->second_half;
    }
};