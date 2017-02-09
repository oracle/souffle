/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/


#include <iostream>
#include "Iteration.hpp"


void Iteration::addRule(std::vector <std::string> data, std::string rec_id) {
    std::string strTemp = data[4] + data[3] + data[2];

    if (data[0].at(0) == 't') {

        if (rul_rec_map.find(strTemp) != rul_rec_map.end()) {
            std::shared_ptr <Rule> rul_rec = rul_rec_map[strTemp];
            rul_rec->setRuntime(std::stod(data[5]) + rul_rec->getRuntime());
        } else {
            std::shared_ptr <Rule> rul_rec = std::make_shared<Rule>(Rule(data[4],
                                                                         std::stoi(data[2]), rec_id));
            std::cout << "Iteration Rule runtime: " << (data[5]);
            std::cout << "\n\tstod(runtime): " << std::stod(data[5]) << std::endl;
            rul_rec->setRuntime(std::stod(data[5]));
            rul_rec->setLocator(data[3]);
            rul_rec_map[strTemp] = rul_rec;
        }

    } else if (data[0].at(0) == 'n') {
        std::unordered_map < std::string, std::shared_ptr < Rule >> ::const_iterator
        got = rul_rec_map.find(strTemp);

        assert(got != rul_rec_map.end() && "missing t tag");
        std::shared_ptr <Rule> rul_rec = rul_rec_map[strTemp];
        rul_rec->setNum_tuples(std::stol(data[5]) - prev_num_tuples);
        this->prev_num_tuples = std::stol(data[5]);
        rul_rec_map[strTemp] = rul_rec;

    }
}

std::string Iteration::toString() {
    std::ostringstream output;

    output << runtime << "," << num_tuples << "," << copy_time << ",";
    output << " recRule:";
    for (auto &rul : rul_rec_map)
        output << rul.second->toString();
    output << "\n";
    return output.str();
}