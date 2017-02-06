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

template<typename T>
class Singleton {
    private:
        Singleton() {}
        static T _instance;
    public:
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
        static const T& getInstance() { return _instance; }
        static void setInstance(const T& instance) { _instance = instance; }
};


}



class MainOption {
    public:
       const std::string longName;
       const char shortName;
       const std::string argument;
       const std::string byDefault;
       const std::string delimiter;
       const std::string description;
};



/** Class to handle the command line arguments. */
class GlobalConfig : public simple::Table<std::string, std::string> {
    private:
        int argc;
        char** argv;
        const std::string header;
        const std::string footer;
        const std::vector<MainOption> mainOptions;
    public:
        GlobalConfig(int argc, char** argv, const std::string header, const std::string footer, const std::vector<MainOption> mainOptions);
        GlobalConfig(const GlobalConfig& other) { data(other.data()); }
        GlobalConfig& operator=(const GlobalConfig& other) { data(other.data()); return *this; }
        void printHelp(std::ostream& os);
        void error();

};


class Global : public simple::Singleton<GlobalConfig> {

};

};