/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "DataComparator.hpp"
#include "Row.hpp"

class Table {
public:
    std::vector<std::shared_ptr<Row>> rows;

    Table() : rows() {}

    void addRow(std::shared_ptr<Row> row) {
        rows.push_back(row);
    }

    inline std::vector<std::shared_ptr<Row>> getRows() {
        return rows;
    }

    void sort(int col_num);
};
