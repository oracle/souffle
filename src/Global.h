#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <ctype.h>
#include <getopt.h>

#include "Util.h"

namespace souffle {

struct MainOption {
       std::string longName;
       char shortName;
       std::string argument;
       std::string byDefault;
       std::string delimiter;
       std::string description;
};

class MainConfig : public BaseTable<std::string, std::string> {
    private:
         std::string _help;
    public:
        MainConfig() : BaseTable<std::string, std::string>() {}
        void processArgs(int argc, char** argv, const std::string header, const std::string footer, const std::vector<MainOption> mainOptions);
        const std::string& help() const { return _help; }
};

class Global {
    private:
        Global() {}
    public:
        Global(const Global&) = delete;
        Global& operator=(const Global&) = delete;
        static MainConfig& config() {
            static MainConfig _config;
            return _config;
        }
};


};