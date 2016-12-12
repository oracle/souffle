#include "Rule.hpp"


std::string Rule::toString() {
    std::ostringstream output;
    if (recursive) {
        output << "{" << name << "," << version << ":";
    } else {
        output << "{" << name << ":";
    }
    output << "[" << runtime << "," << num_tuples << "]}";
    return output.str();
}

void Rule::setLocator(std::string locator) {
    if (this->locator.empty()) {
        this->locator = locator;
    } else {
        this->locator += " " + locator;
    }
}