/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

#pragma once

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cassert>
#include <exception>
#include <memory>
#include <regex>
#include <thread>

#include <dirent.h>

#include "Relation.hpp"
#include "ProgramRun.hpp"
#include "Rule.hpp"
#include "Iteration.hpp"
#include "StringUtils.hpp"


class Reader {
private:
    std::string file_loc;
    std::ifstream file;
    std::ifstream live_file;
    std::ios::streampos gpos;

    bool loaded = false;
    bool online;

    double runtime;
    std::unordered_map <std::string, std::shared_ptr<Relation>> relation_map;
    int rel_id = 0;

public:
    std::shared_ptr <ProgramRun> run;

    Reader(std::string arg, std::shared_ptr <ProgramRun> run, bool vFlag, bool online) :
            file_loc(arg), file(arg), online(online), runtime(-1.0),
            relation_map(std::unordered_map < std::string, std::shared_ptr < Relation >>()) {
        this->run = run;
    }

    /**
     * Read the contents from file into the class
     */
    void readFile();

    void save(std::string f_name);

    void process(const std::vector <std::string> &data);

    inline bool isLive() { return online; }

    void addIteration(std::shared_ptr <Relation> rel, std::vector <std::string> data);

    void addRule(std::shared_ptr <Relation> rel, std::vector <std::string> data);

    inline bool isLoaded() { return loaded; }

    std::string RelationcreateId() { return "R" + std::to_string(++rel_id); }

    std::string createId();

    void livereadinit();
    void liveread();
};
