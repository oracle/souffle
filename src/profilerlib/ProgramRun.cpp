#include "ProgramRun.hpp"


std::string ProgramRun::toString() {
    std::ostringstream output;
    output << "ProgramRun:" << runtime << "\nRelations:\n";
    for (auto r = relation_map.begin(); r != relation_map.end(); ++r) {
        output << r->second->toString() << "\n";
    }
    return output.str();
}

long ProgramRun::getTotNumTuples() {
    long result = 0;
    for (auto& item : relation_map) {
        result += item.second->getTotNum_tuples();
    }
    return result;
}

long ProgramRun::getTotNumRecTuples() {
    long result = 0;
    for (auto& item : relation_map) {
        result += item.second->getTotNumRec_tuples();
    }
    return result;
}

double ProgramRun::getTotCopyTime() {
    double result = 0;
    for (auto& item : relation_map) {
        result += item.second->getCopyTime();
    }
    return result;
}

double ProgramRun::getTotTime() {
    double result = 0;
    for (auto& item : relation_map) {
        result += item.second->getRecTime();
    }
    return result;
}

Relation* ProgramRun::getRelation(std::string name) {
    if(relation_map.find(name) != relation_map.end()) {
        return &(*relation_map[name]);
    }
    return nullptr;
}
