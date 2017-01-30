/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */


/************************************************************************
 *
 * @file IOSystem.h
 *
 ***********************************************************************/

#pragma once

#include <map>
#include <memory>
#include <string>

#include "IODirectives.h"
#include "ReadStream.h"
#include "ReadStreamCSV.h"
#include "ReadStreamSQLite.h"
#include "SymbolMask.h"
#include "SymbolTable.h"
#include "WriteStream.h"
#include "WriteStreamCSV.h"
#include "WriteStreamSQLite.h"

namespace souffle {


class IOSystem {
public:
    static IOSystem& getInstance() {
        static IOSystem singleton;
        return singleton;
    }

    void registerWriteStreamFactory(std::shared_ptr<WriteStreamFactory> factory) {
        outputFactories[factory->getName()] = factory;
    }
    void registerReadStreamFactory(std::shared_ptr<ReadStreamFactory> factory) {
        inputFactories[factory->getName()] = factory;
    }
    /**
     * Return a new WriteStream built based on the options string.
     * Option string should be in the form "key1=value1,key2=value2"
     */
    std::unique_ptr<WriteStream> getWriter(const SymbolMask& symbolMask,
            const SymbolTable &symbolTable, const std::string& options) {
	std::map<std::string, std::string> optionMap = parseOptions(options);
        if (optionMap.count("IO") == 0 || outputFactories.count(optionMap["IO"]) == 0) {
            std::cerr << "Invalid writer <" << optionMap["IO"] << "> was asked for." << std::endl;
            exit(1);
        }
        return outputFactories[optionMap["IO"]]->getWriter(symbolMask, symbolTable, optionMap);
    }
    /**
     * Return a new ReadStream built based on the options string.
     * Option string should be in the form "key1=value1,key2=value2"
     */
    std::unique_ptr<ReadStream> getReader(const SymbolMask& symbolMask,
            SymbolTable &symbolTable, const std::string& options) {
	std::map<std::string, std::string> optionMap = parseOptions(options);
        if (optionMap.count("IO") == 0 || inputFactories.count(optionMap["IO"]) == 0) {
            std::cerr << "Invalid writer <" << optionMap["IO"] << "> was asked for." << std::endl;
            exit(1);
        }
        return inputFactories[optionMap["IO"]]->getReader(symbolMask, symbolTable, optionMap);
    }
 
    /**
     * Return a new WriteStream
     */
    std::unique_ptr<WriteStream> getWriter(
            const SymbolMask& symbolMask, const SymbolTable& symbolTable, const IODirectives& ioDirectives) {
        return outputFactories.at(ioDirectives.getIOType())->getWriter(symbolMask, symbolTable, ioDirectives);
    }
    /**
     * Return a new ReadStream
     * */
    std::unique_ptr<ReadStream> getReader(
            const SymbolMask& symbolMask, SymbolTable symbolTable, const IODirectives& ioDirectives) {
        return inputFactories[ioDirectives.getIOType()]->getReader(symbolMask, symbolTable, ioDirectives);
    }
    ~IOSystem() {}
private:
    /**
     * Parses an input string for options in the form:
     * key1=value1,key2=value2
     */
    std::map<std::string, std::string> parseOptions(const std::string& options) {
        std::map<std::string, std::string> optionMap;
        std::string modifiedOptionString = stripQuotedSections(options);
        size_t pos = 0;
        size_t startPos = 0;
        while (pos < options.length()) {
            pos = modifiedOptionString.find(",", pos);
            if (pos == std::string::npos ) {
                pos = options.length();
            }
            if (pos == startPos) {
                ++pos;
                ++startPos;
                continue;
            }
            size_t equalPos = options.find("=", startPos);
            if (equalPos >= pos || equalPos == std::string::npos) {
                optionMap[trim(options.substr(startPos, pos - startPos))] = "1";
            } else {
                optionMap[trim(options.substr(startPos, equalPos - startPos))]
                          = options.substr(equalPos + 1, pos - equalPos - 1);
            }
            ++pos;
            startPos = pos;
        }
        return optionMap;
    }
    /**
     * Helper function for parseOptions that masks any quoted text.
     */
    std::string stripQuotedSections(const std::string& optionString) {
        std::string modifiedOptionString(optionString);
        size_t pos = 0;
        size_t startPos = 0;
        bool inQuote = false;
        while (true) {
            pos = modifiedOptionString.find('"', pos);
            if (pos == std::string::npos) {
                break;
            }
            if (modifiedOptionString.at(pos - 1) == '\\') {
                ++pos;
                continue;
            }
            if (!inQuote) {
                startPos = pos;
                inQuote = true;
            } else {
                while (startPos <= pos) {
                    modifiedOptionString.at(startPos) = 0;
                }
                inQuote = false;
            }
        }
        return modifiedOptionString;
    }
    std::string trim(std::string str) {
        str.erase(str.begin(), std::find_if(str.begin(), str.end(),
            std::ptr_fun<int, int>(std::isgraph)));
        str.erase(std::find_if(str.rbegin(), str.rend(),
            std::ptr_fun<int, int>(std::isgraph)).base(), str.end());
	return str;
    }
    IOSystem() {
        registerReadStreamFactory(std::make_shared<ReadFileCSVFactory>());
        registerReadStreamFactory(std::make_shared<ReadCinCSVFactory>());
        registerReadStreamFactory(std::make_shared<ReadStreamSQLiteFactory>());

        registerWriteStreamFactory(std::make_shared<WriteFileCSVFactory>());
        registerWriteStreamFactory(std::make_shared<WriteCoutCSVFactory>());
        registerWriteStreamFactory(std::make_shared<WriteSQLiteFactory>());
    };
    std::map<std::string, std::shared_ptr<WriteStreamFactory>> outputFactories;
    std::map<std::string, std::shared_ptr<ReadStreamFactory>> inputFactories;
};

} /* namespace souffle */
