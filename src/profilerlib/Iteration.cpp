

#include "Iteration.hpp"


void Iteration::addRule(std::vector<std::string> data, std::string rec_id) {
    std::string strTemp = data[4] + data[3] + data[2];

    if (data[0].at(0) == 't') {
        if (rul_rec_map.find(strTemp) != rul_rec_map.end()) {
            std::shared_ptr<Rule> rul_rec = rul_rec_map[strTemp];
            rul_rec->setRuntime(std::stod(data[5]) + rul_rec->getRuntime());
        } else {
            std::shared_ptr<Rule> rul_rec = std::make_shared<Rule>(Rule(data[4],
                                std::stoi(data[2]), rec_id));
            rul_rec->setRuntime(std::stod(data[5]));
            rul_rec->setLocator(data[3]);
            rul_rec_map.emplace(strTemp, rul_rec);
        }

    } else if (data[0].at(0) == 'n') {
        std::unordered_map<std::string,std::shared_ptr<Rule>>::const_iterator got = rul_rec_map.find(strTemp);

        assert (got != rul_rec_map.end() && "missing t tag");
        std::shared_ptr<Rule> rul_rec = rul_rec_map[strTemp];
        rul_rec->setNum_tuples(std::stol(data[5]) - prev_num_tuples);
        prev_num_tuples = std::stol(data[5]);
        rul_rec_map.emplace(strTemp, rul_rec);
    }
}

std::string Iteration::toString() {
//    std::ostringstream output;
//
//    output << std::fixed << std::setprecision(1) << std::scientific << runtime << "," << num_tuples << "," << copy_time << ",";
//    output << " recRule:";
//    for (auto &rul : rul_rec_map)
//        output << rul.second->toString();
//    output << "\n";
//    return output.str();

    std::ostringstream output;

    output << std::fixed << std::setprecision(2) << std::scientific << runtime << "," << num_tuples << "," << copy_time << ",";
    output << " [\"recRule\",";
    for (auto &rul : rul_rec_map)
        output << rul.second->toString();
    output << "]\n";
    return output.str();
}