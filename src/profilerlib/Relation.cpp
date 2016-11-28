

#include "Relation.hpp"


std::vector<std::shared_ptr<Rule>> Relation::getRuleRecList() {
    std::vector<std::shared_ptr<Rule>> temp = std::vector<std::shared_ptr<Rule>>();
    for (auto &iter : iterations) {
        for (auto &rul : iter->getRul_rec()) {
            temp.push_back(rul.second);
        }
    }
    return temp;
}


std::string Relation::createRecID(std::string name) {
    for (auto &iter : iterations) {
        for (auto &rul : iter->getRul_rec()) {
            if (rul.second->getName().compare(name)==0) {
                return rul.second->getId();
            }
        }
    }
    rec_id++;
    return "C" + id.substr(1) + "." + std::to_string(rec_id);
}


double Relation::getRecTime() {
    double result = 0;
    for (auto &iter : iterations) {
        result += iter->getRuntime();
    }
    return result;
}

double Relation::getCopyTime() {
    double result = 0;
    for (auto &iter : iterations) {
        result += iter->getCopy_time();
    }
    return result;
}

long Relation::getNum_tuplesRel() {
    long result = 0L;
    for (auto &iter : iterations) {
        result += iter->getNum_tuples();
    }
    return num_tuples + result;
}

long Relation::getNum_tuplesRul() {
    long result = 0L;
    for (auto &rul : ruleMap) {
        result += rul.second->getNum_tuples();
    }
    for (auto &iter : iterations) {
        for (auto &rul : iter->getRul_rec()) {
            result += rul.second->getNum_tuples();
        }
    }
    return result;
}

long Relation::getTotNumRec_tuples() {
    long result = 0L;
    for (auto &iter : iterations) {
        for (auto &rul : iter->getRul_rec()) {
            result += rul.second->getNum_tuples();
        }
    }
    return result;
}


std::string Relation::toString() {
    std::ostringstream output;
    output << "{\n\"" << name << "\":[" << runtime << "," << num_tuples
           << "],\n\n\"onRecRules\":[\n";
    for (auto &rul : ruleMap) {
        output << rul.second->toString();
    }
    // TODO: ensure this is the same as java, as java just prints an array
    output << "\n],\n\"iterations\":\n";
    output << "[";
    if (iterations.empty()) {
        output << ", ";
    }
    for (auto &iter : iterations) {
        output << iter->toString();
        output << ", ";
    }
    std::string retStr = output.str();
    //substring to remove the last comma
    return retStr.substr(0, retStr.size()-2) + "]\n}";

//    std::ostringstream output;
//    output << "{\n" << name << ":" << runtime << ";" << num_tuples
//           << "\n\nonRecRules:\n";
//    for (auto &rul : ruleMap) {
//        output << rul.second->toString();
//    }
//    // TODO: ensure this is the same as java, as java just prints an array
//    output << "\n\niterations:\n";
//    output << "[";
//    if (iterations.empty()) {
//        output << ", ";
//    }
//    for (auto &iter : iterations) {
//        output << iter->toString();
//        output << ", ";
//    }
//    std::string retStr = output.str();
//    //substring to remove the last comma
//    return retStr.substr(0, retStr.size()-2) + "]\n}";
}