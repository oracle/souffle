#include "ProgramRun.hpp"


std::string ProgramRun::toString() {
    std::ostringstream output;
    output << "ProgramRun:" << runtime << "\nRelations:\n";
    for (auto it = relation_map.begin(); it != relation_map.end(); ++it) {
        output << it->second->toString() << "\n";
    }
    return output.str();
}