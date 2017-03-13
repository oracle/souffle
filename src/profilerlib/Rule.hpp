/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

#pragma once

#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

/*
 * Class to hold information about souffle Rule profile information
 */
class Rule {
protected:
    std::string name;
    double runtime = 0;
    long num_tuples = 0;
    std::string identifier;
    std::string locator = "";

private:
    bool recursive = false;
    int version;

public:
    Rule(std::string name, std::string id) : name(name), identifier(id) {}

    Rule(std::string name, int version, std::string id)
            : name(name), identifier(id), recursive(true), version(version) {}

    inline std::string getId() {
        return identifier;
    }

    inline double const getRuntime() {
        return runtime;
    }

    inline long getNum_tuples() {
        return num_tuples;
    }

    inline void setRuntime(double runtime) {
        this->runtime = runtime;
    }

    inline void setNum_tuples(long num_tuples) {
        this->num_tuples = num_tuples;
    }

    inline std::string getName() {
        return name;
    }

    inline void setId(std::string id) {
        identifier = id;
    }

    inline std::string getLocator() {
        return locator;
    }

    void setLocator(std::string locator);

    inline bool isRecursive() {
        return recursive;
    }

    inline void setRecursive(bool recursive) {
        this->recursive = recursive;
    }

    inline int getVersion() {
        return version;
    }

    inline void setVersion(int version) {
        this->version = version;
    }

    std::string toString();
};
