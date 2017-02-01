/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */


/************************************************************************
 *
 * @file IODirectives.h
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
        return get("IO");
    }

    void setIOType(const std::string& type) {
        directives["IO"] = type;
    }

    char getDelimiter() const {
        return get("delimiter").at(0);
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
        return get("compress") != "false";
    }

    const std::string& getFileName() const {
        return get("filename");
    }

    void setFileName(const std::string& filename) {
        directives["filename"] = filename;
    }

    const std::string& getRelationName() const {
        return get("name");
    }

    void setRelationName(const std::string& name) {
        if (directives.count("filename") == 0) {
            directives["filename"] = name + ".facts";
        }
        directives["name"] = name;
    }

    const std::string& getDBName() const {
        return get("dbname");
    }

    bool isSet() {
        return set;
    }

    void print(std::ostream& out) const {
        auto cur = directives.begin();
        if (cur == directives.end()) {
            return;
        }

        out << "{{\"" << cur->first << "\",\"" << escape(cur->second) << "\"}";
        ++cur;
        for(;cur != directives.end(); ++cur) {
            out << ",{\"" << cur->first << "\",\"" << escape(cur->second) << "\"}";
        }
        out << '}';
    }

    friend std::ostream& operator<<(std::ostream& out, const IODirectives ioDirectives) {
        ioDirectives.print(out);
        return out;
    }

private:
    void setDefaults() {
        directives["IO"] = "file";
        directives["delimiter"] = "\t";
        directives["compress"] = "false";
    }
    const std::string& get(const std::string& key) const {
        if (directives.count(key) == 0) {
            throw std::invalid_argument("Requested IO directive <" + key + "> was not specified");
        }
        return directives.at(key);
    }

    std::string escape(const std::string& inputString) const {
        std::string escaped = escape(inputString, "\"", "\\\"");
        escaped = escape(escaped, "\t", "\\t");
        escaped = escape(escaped, "\r", "\\r");
        escaped = escape(escaped, "\n", "\\n");
        return escaped;
    }

    std::string escape(
            const std::string& inputString, const std::string& needle, const std::string replacement) const {
        std::string result = inputString;
        size_t pos = 0;
        while ((pos = result.find(needle, pos)) != std::string::npos) {
            result = result.replace(pos, needle.length(), replacement);
            pos += replacement.length();
        }
        return result;
    }

    std::map<std::string, std::string> directives;
    bool set = false;
};

}
