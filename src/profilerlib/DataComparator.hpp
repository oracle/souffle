/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

#pragma once

#include "CellInterface.hpp"
#include "Row.hpp"

#include <cmath>
#include <memory>
#include <vector>

/*
 * Data comparison functions for sorting tables
 *
 * Will sort the values of only one column, in descending order
 *
 * TODO: sort IDs by ID number
 * TODO: sort in both ascending and descending
 * TODO: ensure NaN values are sorted correctly (only tried doubles... only one necessary?)
 *
 * Could be extended to save a few variables to allow for ascending sort and multiple columns
 * However not really necessary
 */
class DataComparator {
public:
    /* descending order comparator used to sort rows */
    static bool TIME(std::shared_ptr<Row> a, std::shared_ptr<Row> b) { // TOT_T: total time
        return compare_doubles(a->cells[0]->getDoubVal(), b->cells[0]->getDoubVal());
    }

    static bool NR_T(std::shared_ptr<Row> a, std::shared_ptr<Row> b) { // NREC_T: non recursive time
        return compare_doubles(a->cells[1]->getDoubVal(), b->cells[1]->getDoubVal());
    }

    static bool R_T(std::shared_ptr<Row> a, std::shared_ptr<Row> b) { // REC_T: recursive time
        return compare_doubles(a->cells[2]->getDoubVal(), b->cells[2]->getDoubVal());
    }

    static bool C_T(std::shared_ptr<Row> a, std::shared_ptr<Row> b) { // COPY_T: copy time
        return compare_doubles(a->cells[3]->getDoubVal(), b->cells[3]->getDoubVal());
    }

    static bool TUP(std::shared_ptr<Row> a, std::shared_ptr<Row> b) { // Tuples
        return b->cells[4]->getLongVal() < a->cells[4]->getLongVal();
    }

    static bool NAME(std::shared_ptr<Row> a, std::shared_ptr<Row> b) { // Name
        return b->cells[5]->getStringVal() > a->cells[5]->getStringVal();
    }

    static bool ID(std::shared_ptr<Row> a, std::shared_ptr<Row> b) { // ID
        // TODO: compare the actual ID values
        return b->cells[6]->getStringVal() > a->cells[6]->getStringVal();
    }

    // sort doubles by pointer reference if the values are NaN so ordering of values are the same
    // TODO: add the same for infinite... shouldn't be necessary for souffle
    static bool compare_doubles(double a, double b) {
        if (std::isnan(a)) {
            if (std::isnan(b)) {
                return &b > &a;
            }
            return false;
        }
        if (std::isnan(b)) {
            return true;
        }
        return b < a;
    }
};
