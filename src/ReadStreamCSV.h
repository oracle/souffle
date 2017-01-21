/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file ReadStreamCSV.h
 *
 ***********************************************************************/

#pragma once

#include "ReadStream.h"

#include "RamTypes.h"
#include "SymbolMask.h"
#include "SymbolTable.h"

#include <fstream>
#include <map>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>

namespace souffle {

class ReadStreamCSV : public ReadStream {
public:
    ReadStreamCSV(std::istream& in, const SymbolMask& symbolMask, SymbolTable& symbolTable,
            std::map<int, int> inputMap = std::map<int, int>(), char delimiter = '\t')
            : delimiter(delimiter),
              file(in),
              lineNumber(0),
              symbolMask(symbolMask),
              symbolTable(symbolTable),
              inputMap(inputMap) {
        while (this->inputMap.size() < symbolMask.getArity()) {
            int size = this->inputMap.size();
            this->inputMap[size] = size;
        }
    }

    /**
     * Read and return the next tuple.
     *
     * Returns nullptr if no tuple was readable.
     * @return
     */
    virtual std::unique_ptr<RamDomain[]> readNextTuple() {
        if (file.eof()) {
            return nullptr;
        }
        std::string line;
        std::unique_ptr<RamDomain[]> tuple(new RamDomain[symbolMask.getArity()]);
        bool error = false;

        if (!getline(file, line)) {
            return nullptr;
        }
        ++lineNumber;

        size_t start = 0, end = 0, columnsFilled = 0;
        for (uint32_t column = 0; end < line.length(); column++) {
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
                    std::stringstream errorMessage;
                    errorMessage << "Value missing in column " << column + 1 << " in line " << lineNumber
                                 << "; ";
                    throw std::invalid_argument(errorMessage.str());
                }
                element = "n/a";
            }
            start = end + 1;
            if (inputMap.count(column) == 0) {
                continue;
            }
            ++columnsFilled;
            if (symbolMask.isSymbol(column)) {
                tuple[inputMap[column]] = symbolTable.lookup(element.c_str());
            } else {
                try {
                    tuple[inputMap[column]] = std::stoi(element.c_str());
                } catch (...) {
                    if (!error) {
                        std::stringstream errorMessage;
                        errorMessage << "Error converting number in column " << column + 1 << " in line "
                                     << lineNumber << "; ";
                        throw std::invalid_argument(errorMessage.str());
                    }
                }
            }
        }
        if (columnsFilled != symbolMask.getArity()) {
            std::stringstream errorMessage;
            errorMessage << "Values missing in line " << lineNumber << "; ";
            throw std::invalid_argument(errorMessage.str());
        }
        if (end != line.length()) {
            if (!error) {
                std::stringstream errorMessage;
                errorMessage << "Too many cells in line " << lineNumber << "; ";
                throw std::invalid_argument(errorMessage.str());
            }
        }
        if (error) {
            throw std::invalid_argument("cannot parse fact file");
        }

        return tuple;
    }

    virtual ~ReadStreamCSV() {}

private:
    const char delimiter;
    std::istream& file;
    size_t lineNumber;
    const SymbolMask& symbolMask;
    SymbolTable& symbolTable;
    std::map<int, int> inputMap;
};

class ReadFileCSV : public ReadStream {
public:
    ReadFileCSV(const std::string& filename, const SymbolMask& symbolMask, SymbolTable& symbolTable,
            std::map<int, int> inputMap = std::map<int, int>(), char delimiter = '\t')
            : fileHandle(filename), readStream(fileHandle, symbolMask, symbolTable, inputMap, delimiter) {
        char bfn[filename.size()];
        strcpy(bfn, filename.c_str());
        std::stringstream baseNameStream(basename(bfn));
        baseName = baseNameStream.str();
        if (!fileHandle.is_open()) {
            char bfn[filename.size()];
            strcpy(bfn, filename.c_str());
            std::stringstream errorMessage;
            errorMessage << "Cannot open fact file " << baseName << "\n";
            throw std::invalid_argument(errorMessage.str());
        }
    }
    /**
     * Read and return the next tuple.
     *
     * Returns nullptr if no tuple was readable.
     * @return
     */
    virtual std::unique_ptr<RamDomain[]> readNextTuple() {
        try {
            return readStream.readNextTuple();
        } catch (std::exception& e) {
            std::stringstream errorMessage;
            errorMessage << e.what();
            errorMessage << "cannot parse fact file " << baseName << "!\n";
            throw std::invalid_argument(errorMessage.str());
        }
    }

    virtual ~ReadFileCSV() {}

private:
    std::string baseName;
    std::ifstream fileHandle;
    ReadStreamCSV readStream;
};

class ReadCSVFactory {
protected:
    char getDelimiter(const std::map<std::string, std::string>& options) {
        char delimiter = '\t';
        if (options.count("delimiter") > 0) {
            delimiter = options.at("delimiter").at(0);
        }
        return delimiter;
    }
};

class ReadCinCSVFactory : public ReadStreamFactory, public ReadCSVFactory {
public:
    std::unique_ptr<ReadStream> getReader(const SymbolMask& symbolMask, SymbolTable& symbolTable,
            const std::map<std::string, std::string>& options) {
        return std::unique_ptr<ReadStreamCSV>(new ReadStreamCSV(
                std::cin, symbolMask, symbolTable, std::map<int, int>(), getDelimiter(options)));
    }
    virtual ~ReadCinCSVFactory() {}
};

class ReadFileCSVFactory : public ReadStreamFactory, public ReadCSVFactory {
public:
    std::unique_ptr<ReadStream> getReader(const SymbolMask& symbolMask, SymbolTable& symbolTable,
            const std::map<std::string, std::string>& options) {
        std::map<int, int> inputMap;
        if (options.count("columns") > 0) {
            std::istringstream iss(options.at("columns"));
            std::string mapping;
            int index = 0;
            while (std::getline(iss, mapping, ':')) {
                // TODO (mmcgr): handle ranges like 4-7
                inputMap[stoi(mapping)] = index++;
            }
            if (inputMap.size() < symbolMask.getArity()) {
                throw std::invalid_argument("Invalid column set was given: <" + options.at("columns") + ">");
            }
        } else {
            while (inputMap.size() < symbolMask.getArity()) {
                int size = inputMap.size();
                inputMap[size] = size;
            }
        }
        return std::unique_ptr<ReadFileCSV>(new ReadFileCSV(
                options.at("name"), symbolMask, symbolTable, inputMap, getDelimiter(options)));
    }
    virtual ~ReadFileCSVFactory() {}
};

} /* namespace souffle */
