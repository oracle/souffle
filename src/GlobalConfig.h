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

/** A simple table class. */
class StringTable {

    private:

        /** An empty string object, used to return by reference. */
        const std::string _empty;

        /** A data of key value pairs. */
        std::map<std::string, std::string> _data;

    public:

        /** Constructor for the data class. */
        StringTable() : _empty(std::string()), _data(std::map<std::string, std::string>())  {}

        /** Copy constructor for the data class. */
        StringTable(const StringTable& other) { data(other.data()); }

        /** Assignment operator for the data class. */
        StringTable& operator=(const StringTable& other) { data(other.data()); return *this; }

        const std::map<std::string, std::string>& data() const { return _data; }

        void data(const std::map<std::string, std::string>& rhs) { _data = rhs; }

        /** Get the value for a given key from the data, returning an _empty string if it does not exist. */
        const std::string& get(const std::string& key) const { return (has(key)) ? _data.at(key) : _empty; }

        /** Get the value for a given key from the data, returning the specified value if it does not exist. */
        const std::string& get(const std::string& key, const std::string& value) const  { return (has(key)) ? _data.at(key) : value; }

        /** Check if the data has any value for the given key. */
        const bool has(const std::string& key) const { return _data.find(key) != _data.end(); }

        /** Check if the data has the specified value for the given key. */
        const bool has(const std::string& key, const std::string& value) const { return has(key) && _data.at(key) == value; }

        /** Set the value for a key in the data to an _empty string. */
        void set(const std::string& key) { _data[key] = _empty; }

        /** Set the value for a key in the data to the given value. */
        void set(const std::string& key, const std::string& value) { _data[key] = value; }

        /** Print the data to the specified output stream. */
        void print(std::ostream& os) { os << _data << std::endl; }
};

struct MainOption {
       const std::string longName;
       const char shortName;
       const std::string argumentType;
       const std::string defaultValue;
       const bool takesManyArguments;
       const std::string description;
};

/** Class to handle the command line arguments. */
class GlobalConfig : public StringTable {

    private:

        /** Argument count, passed from main. */
        int argc;

        /** Argument array, passed from main. */
        char** argv;

        /** Header, for the help text. */
        const std::string header;

        /** Footer, for the help text. */
        const std::string footer;

        /** Options, for both the command line arguments and the help text. */
        const std::vector<MainOption> mainOptions;


    public:

        /** Empty constructor for the environment. */
        GlobalConfig();

        /** Constructor for the environment. */
        GlobalConfig(int argc, char** argv, const std::string header, const std::string footer, const std::vector<MainOption> mainOptions);

        /** Copy constructor for the environment. */
        GlobalConfig(const GlobalConfig& other) { data(other.data()); }

        /** Assignment operator for the environment. */
        GlobalConfig& operator=(const GlobalConfig& other) { data(other.data()); return *this; }

        /** Print the help message to the given output stream. */
        void printHelp(std::ostream& os);

        /** Print an error message, the help text, and exit. */
        void error();

};

};