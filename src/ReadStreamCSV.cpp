#include "ReadStreamCSV.h"

#include "RamTypes.h"

#include <string>

namespace souffle {

bool ReadStreamCSV::hasNextTuple() {
    // This is only correct if the file is in the expected format.
    return !file.eof();
}

std::unique_ptr<RamDomain[]> ReadStreamCSV::readNextTuple() {
    if (file.eof()) {
        return nullptr;
    }
    std::string line;
    std::unique_ptr<RamDomain[]> tuple(new RamDomain[symbolMask.getArity()]);
    bool error = false;

    if(!getline(file, line)) {
            std::cerr << "Reading input failed at line " << lineNumber << ";";
    }
    ++lineNumber;

    size_t start = 0, end = 0;
    for (uint32_t column = 0; column < symbolMask.getArity(); column++) {
        end = line.find(delimiter, start);
        if (end == std::string::npos) {
            end = line.length();
        }
        std::string element;
        if (start <= end && end <= line.length()) {
            element = line.substr(start, end - start);
            if (element == "") {
                element = "n/a";
            }
        } else {
            if (!error) {
                std::cerr << "Value missing in column " << column + 1
                        << " in line " << lineNumber << "; ";
                error = true;
            }
            element = "n/a";
        }
        if (symbolMask.isSymbol(column)) {
            tuple[column] = symbolTable.lookup(element.c_str());
        } else {
            try {
                tuple[column] = std::stoi(element.c_str());
            } catch (...) {
                if (!error) {
                    std::cerr << "Error converting number in column " << column + 1
                            << " in line " << lineNumber << "; ";
                    error = true;
                }
            }
        }
        start = end + 1;
    }
    if (end != line.length()) {
        if (!error) {
            std::cerr << "Too many cells in line " << lineNumber << "; ";
            error = true;
        }
    }
    if (error) {
        return std::unique_ptr<RamDomain[]>(nullptr);
    }

    return tuple;
}

}
