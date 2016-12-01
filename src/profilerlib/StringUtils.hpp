//
// Created by Dominic Romanowski on 30/11/16.
//

#pragma once

#include <string>
#include <vector>
#include <cmath>

#include "Table.hpp"

namespace Tools {
    static const std::string arr[] = {"K","M","B","t","q","Q","s","S","o","n","d","U"};
    static const std::vector<std::string> abbreviations(arr,arr+sizeof(arr)/sizeof(arr[0]));

//    static typedef std::tuple<double, double, double, double, double, std::string, double, double, double> rel_table_vals;

    std::string formatNum(int precision, long amount);

    std::string formatTime(double number);

    std::vector<std::vector<std::string>> formatTable(Table table, int precision);
}