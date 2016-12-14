/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include "Relation.hpp"
#include "StringUtils.hpp"


class ProgramRun {
private:
    std::unordered_map <std::string, std::shared_ptr<Relation>> relation_map;
    int rel_id = 0;
    double runtime;
    double tot_rec_tup = 0.0;
    double tot_copy_time = 0.0;
public:
    ProgramRun() : relation_map(),
                   runtime(-1.0) {}

    inline void SetRuntime(double runtime) { this->runtime = runtime; }

    inline void setRelation_map(std::unordered_map <std::string, std::shared_ptr<Relation>> &relation_map) {
        this->relation_map = relation_map;
    }

    inline void update() {
        tot_rec_tup = (double) getTotNumRecTuples();
        tot_copy_time = getTotCopyTime();
    };

    std::string toString();

    inline std::unordered_map <std::string, std::shared_ptr<Relation>> &getRelation_map() { return relation_map; }


    std::string getRuntime() {
        if (runtime == -1.0) {
            return "--";
        }
        return formatTime(runtime);
    }

    long getTotNumTuples();

    long getTotNumRecTuples();

    double getTotCopyTime();

    double getTotTime();

    Relation *getRelation(std::string name);

    inline std::string formatTime(double runtime) { return Tools::formatTime(runtime); }

    inline std::string formatNum(int precision, long number) { return Tools::formatNum(precision, number); }

    inline std::vector <std::vector<std::string>> formatTable(Table table, int precision) {
        return Tools::formatTable(table, precision);
    }

};
