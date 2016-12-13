//
// Created by Dominic Romanowski on 30/11/16.
//

#pragma once

#include <string>
#include <vector>
#include <cmath>
#include <regex>
#include <fstream>

#include <sys/types.h> // required for stat.h
#include <sys/stat.h>

#include <stdio.h>  /* defines FILENAME_MAX */

#ifdef WINDOWS
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif


#include "Table.hpp"

namespace Tools {
    static const std::string arr[] = {"K","M","B","t","q","Q","s","S","o","n","d","U"};
    static const std::vector<std::string> abbreviations(arr,arr+sizeof(arr)/sizeof(arr[0]));

//    static typedef std::tuple<double, double, double, double, double, std::string, double, double, double> rel_table_vals;

    std::string formatNum(int precision, long amount);

    std::string formatTime(double number);

    std::vector<std::vector<std::string>> formatTable(Table table, int precision);

    std::vector<std::string> split(std::string str, std::string split_reg);
    std::vector<std::string> splitAtSemiColon(std::string str);

    inline bool file_exists (const std::string& name) {
        std::ifstream f(name.c_str());
        return f.good();
    }

    std::string getworkingdir();
}