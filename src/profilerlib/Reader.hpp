/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

#pragma once

#include <cassert>
#include <exception>
#include <fstream>
#include <iostream>
#include <memory>
#include <regex>
#include <thread>
#include <unordered_map>

#include <dirent.h>

#include "Iteration.hpp"
#include "ProgramRun.hpp"
#include "Relation.hpp"
#include "Rule.hpp"
#include "StringUtils.hpp"

/*
 * Input reader and processor for log files
 * Contains both offline and live reader
 * TODO: remove offline reader and use live reader
 *  - live reader has the same functionality as offline, but reads from last read position after it reaches
 * EOF
 * TODO: add code to inform UserInputReader to deal with the warning message when live reader finishes
 */
class Reader {
private:
    std::string file_loc;
    std::ifstream file;
    std::ios::streampos gpos;

    bool loaded = false;
    bool online;

    double runtime;
    std::unordered_map<std::string, std::shared_ptr<Relation>> relation_map;
    int rel_id = 0;

public:
    std::shared_ptr<ProgramRun> run;

    Reader(std::string arg, std::shared_ptr<ProgramRun> run, bool vFlag, bool online)
            : file_loc(arg), file(arg), online(online), runtime(-1.0),
              relation_map(std::unordered_map<std::string, std::shared_ptr<Relation>>()) {
        this->run = run;
    }

    /**
     * Read the contents from file into the class
     */
    void readFile();

    void save(std::string f_name);

    void process(const std::vector<std::string>& data);

    inline bool isLive() {
        return online;
    }

    void addIteration(std::shared_ptr<Relation> rel, std::vector<std::string> data);

    void addRule(std::shared_ptr<Relation> rel, std::vector<std::string> data);

    inline bool isLoaded() {
        return loaded;
    }

    std::string RelationcreateId() {
        return "R" + std::to_string(++rel_id);
    }

    std::string createId();

    void livereadinit();
    void liveread();
};
