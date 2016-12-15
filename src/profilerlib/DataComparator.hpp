/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/


#pragma once

#include <vector>
#include <math.h>
#include "CellInterface.hpp"

class DataComparator {
public:
    static bool TIME(std::shared_ptr <Row> a, std::shared_ptr <Row> b) {
        double val1 = a->cells[0]->getDoubVal();
        double val2 = b->cells[0]->getDoubVal();
        if (std::isnan(val1)) {
            return true;
        }
        if (std::isnan(val2)) {
            return false;
        }
        return val2 < val1;
    }

    static bool NR_T(std::shared_ptr <Row> a, std::shared_ptr <Row> b) {
        return b->cells[1]->getDoubVal() < a->cells[1]->getDoubVal();
    }

    static bool R_T(std::shared_ptr <Row> a, std::shared_ptr <Row> b) {
        return b->cells[2]->getDoubVal() < a->cells[2]->getDoubVal();
    }

    static bool C_T(std::shared_ptr <Row> a, std::shared_ptr <Row> b) {
        return b->cells[3]->getDoubVal() < a->cells[3]->getDoubVal();
    }

    static bool TUP(std::shared_ptr <Row> a, std::shared_ptr <Row> b) {
        return b->cells[4]->getLongVal() < a->cells[4]->getLongVal();
    }

    static bool NAME(std::shared_ptr <Row> a, std::shared_ptr <Row> b) {
        return b->cells[5]->getStringVal() > a->cells[5]->getStringVal();
    }

    static bool ID(std::shared_ptr <Row> a, std::shared_ptr <Row> b) {
        return b->cells[6]->getStringVal() > a->cells[6]->getStringVal();
    }
};
