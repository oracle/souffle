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
#include <sstream>
#include <string>

namespace souffle {

class IODirectives {
public:
    IODirectives() {
        setDefaults();
    }

    IODirectives(const std::map<std::string, std::string>& directiveMap) {
        setDefaults();
        for (const auto& pair : directiveMap) {
                directives[pair.first] = pair.second;
        }
        set = !directiveMap.empty();
    }

    ~IODirectives() {}

    const std::string& getIOType() const {
        return directives.at("IO");
    }

    char getDelimiter() const {
        return directives.at("delimiter").at(0);
    }

    std::map<int, int> getColumnMap() const {
        std::map<int, int> columnMap;
        if (directives.count("columns") == 0) {
            return columnMap;
        }
        std::istringstream iss(directives.at("columns"));
        std::string mapping;
        int index = 0;
        while (std::getline(iss, mapping, ':')) {
            // TODO (mmcgr): handle ranges like 4-7
            columnMap[stoi(mapping)] = index++;
        }

		return columnMap;
    }

    bool shouldCompress() const {
        return directives.at("compress") != "false";
    }

    const std::string& getFileName() const { 
        return directives.at("filename");
    }

    void setFileName(const std::string& filename) {
        directives["filename"] = filename;
    }

    const std::string& getRelationName() const {
        return directives.at("name");
    }

    void setRelationName(const std::string& name) {
        directives["name"] = name;
    }

    const std::string& getDBName() const {
        return directives.at("dbname");
    }

    bool isSet() {
        return set;
    }

private:
    void setDefaults() {
        directives["IO"] = "file";
        directives["delimiter"] = "\t";
        directives["compress"] = "false";
    }
    std::map<std::string, std::string> directives;
    bool set = false;
};

}
