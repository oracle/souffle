/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

#include "Table.hpp"

void Table::sort(int col_num) {
    switch (col_num) {
        case 1:
            std::sort(rows.begin(), rows.end(), DataComparator::NR_T);
            break;
        case 2:
            std::sort(rows.begin(), rows.end(), DataComparator::R_T);
            break;
        case 3:
            std::sort(rows.begin(), rows.end(), DataComparator::C_T);
            break;
        case 4:
            std::sort(rows.begin(), rows.end(), DataComparator::TUP);
            break;
        case 5:
            std::sort(rows.begin(), rows.end(), DataComparator::ID);
            break;
        case 6:
            std::sort(rows.begin(), rows.end(), DataComparator::NAME);
            break;
        case 0:
        default:  // if the col_num isn't defined just use TIME... TODO: consider printing warning?
            std::sort(rows.begin(), rows.end(), DataComparator::TIME);
            break;
    }
}
