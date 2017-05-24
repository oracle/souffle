/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "Cell.hpp"
#include "ProgramRun.hpp"
#include "Row.hpp"
#include "StringUtils.hpp"
#include "Table.hpp"

/*
 * Class to format profiler data structures into tables
 */
class OutputProcessor {
private:
    std::shared_ptr<ProgramRun> programRun;

public:
    OutputProcessor() {
        programRun = std::make_shared<ProgramRun>(ProgramRun());
    }

    inline std::shared_ptr<ProgramRun>& getProgramRun() {
        return programRun;
    }

    Table getRelTable();

    Table getRulTable();

    Table getVersions(std::string strRel, std::string strRul);

    std::string formatTime(double number) {
        return Tools::formatTime(number);
    }

    std::string formatNum(int precision, long number) {
        return Tools::formatNum(precision, number);
    }

    std::vector<std::vector<std::string>> formatTable(Table table, int precision) {
        return Tools::formatTable(table, precision);
    }
};