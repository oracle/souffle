#pragma once

#include <string>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <vector>
#include <ctype.h>
#include <getopt.h>

#include "Util.h"

namespace souffle {

namespace simple {

template<typename K, typename V>
class Table {
    private:
        const V _default;
        std::unordered_map<K, V> _data;
    public:
        Table() : _default(V()), _data(std::unordered_map<K, V>()) {}
        Table(const Table& other) { data(other.data()); }
        Table& operator=(const Table& other) { data(other.data()); return *this; }
        const std::unordered_map<K, V>& data() const { return _data; }
        void data(const std::unordered_map<K, V>& otherData) { _data = otherData; }
        const V& get(const K& key) const { return (has(key)) ? _data.at(key) : _default; }
        const V& get(const K& key, const V& value) const  { return (has(key)) ? _data.at(key) : value; }
        const bool has(const K& key) const { return _data.find(key) != _data.end(); }
        const bool has(const K& key, const V& value) const { return has(key) && _data.at(key) == value; }
        void set(const K& key) { _data[key] = _default; }
        void set(const K& key, const V& value) { _data[key] = value; }
        void print(std::ostream& os) { os << _data << std::endl; }
};




}



struct MainOption {
       std::string longName;
       char shortName;
       std::string argument;
       std::string byDefault;
       std::string delimiter;
       std::string description;
};



/** Class to handle the command line arguments. */
class GlobalConfig : public simple::Table<std::string, std::string> {
    private:
        int argc;
        char** argv;
        std::string header;
        std::string footer;
        std::vector<MainOption> mainOptions;
        void processArgs();
    public:
        GlobalConfig();
        void initialize(int argc, char** argv, const std::string header, const std::string footer, const std::vector<MainOption> mainOptions);
        void printHelp(std::ostream& os);
        void error();

};

class Global {
    private:
        Global() {}
    public:
        Global(const Global&) = delete;
        Global& operator=(const Global&) = delete;
        static GlobalConfig& getInstance() {
            static GlobalConfig _instance;
            return _instance;
        }
};


};