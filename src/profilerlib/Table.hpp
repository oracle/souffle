//
// Created by Dominic Romanowski on 30/11/16.
//

#pragma once

#include <vector>
#include <string>
#include <Algorithm>

#include "Row.hpp"
#include "DataComparator.hpp"

class Table {
public:
    std::vector<std::shared_ptr<Row>> rows;
    Table() : rows() {

    }

    void addRow(std::shared_ptr<Row> row) {
        rows.push_back(row);
    }

    inline std::vector<std::shared_ptr<Row>> getRows() {
        return rows;
    }

    void sort(int col_num);
};

