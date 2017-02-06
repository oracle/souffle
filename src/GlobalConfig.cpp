#include "GlobalConfig.h"

namespace souffle {

GlobalConfig::GlobalConfig()
    : StringTable()
{

}

GlobalConfig::GlobalConfig(int argc, char** argv, const std::string header, const std::string footer, const std::vector<Option> options)
    : StringTable()
    , argc (argc)
    , argv (argv)
    , header (header)
    , footer (footer)
    , options (options)
{
    option longOptions[options.size() + 1];
    std::map<const char, std::string> optionTable; // table to map flags to options
    std::map<const char, bool> argumentTable; // table to map flags to argument quantities
    int i = 0;
    std::string shortOptions = "";
    for (const Option& opt : options) {
        longOptions[i] = (option) { opt.name.c_str(), (!opt.argument.empty()), nullptr, opt.flag };
        optionTable[opt.flag] = opt.name;
        argumentTable[opt.flag] = opt.takes_many;
        shortOptions += opt.flag;
        if (!opt.argument.empty()) {
            shortOptions += ":";
        }
        if (!opt.by_default.empty()) set(opt.name, opt.by_default);
        ++i;
    }
    longOptions[i] = {nullptr, false, nullptr, 0}; // the terminal option, needs to be null

    int c;     /* command-line arguments processing */
    while ((c = getopt_long(argc, argv, shortOptions.c_str(), longOptions, nullptr)) != EOF) {
        if (c == '?') error();
        auto iter = optionTable.find(c);
        if (iter == optionTable.end()) {
            ASSERT("Default label in getopt switch.");
            abort();
        }
        std::string arg = (optarg) ? std::string(optarg) : std::string();
        if (argumentTable[c]) {
            set(iter->second, get(iter->second) + " " + arg);
        } else {
            set(iter->second, arg);
        }
    }

}

void GlobalConfig::printHelp(std::ostream& os) {

    // print the header
    os << header;

    // iterate over the options and obtain the maximum name and argument lengths
    int maxLongNameLength = 0, maxArgumentIdLength = 0;
    for (const Option& opt : options) {
        // if it is the main option, do nothing
        if (opt.longName == "") continue;
        // otherwise, proceed with the calculation
        maxLongNameLength = ((int) opt.longName.size() > maxLongNameLength) ? opt.longName.size() : maxLongNameLength;
        maxArgumentIdLength = ((int) opt.argumentId.size() > maxArgumentIdLength) ? opt.argumentId.size() : maxArgumentIdLength;
    }

    // iterator over the options and pretty print them, using padding as determined
    // by the maximum name and argument lengths
    int length;
    for (const Option& opt : options) {

        // if it is the main option, do nothing
        if (opt.longName == "") continue;

        // print the short form name and the argument parameter
        length = 0;
        os << "\t";
        if (isalpha(opt.shortName)) {
            os << "-" << opt.shortName;
            if (!opt.argumentId.empty()) {
                os << "<" << opt.argumentId << ">";
                length = opt.argumentId.size() + 2;
            }
        } else {
            os << "  ";
        }

        // pad with empty space for prettiness
        for (; length < maxArgumentIdLength + 2; ++length) os << " ";

        // print the long form name and the argument parameter
        length = 0;
        os << "\t" << "--" << opt.longName;
        if (!opt.argumentId.empty()) {
            os << "=<" << opt.argumentId << ">";
            length = opt.argumentId.size() + 3;
        }

        // again, pad with empty space for prettiness
        for (length += opt.longName.size(); length < maxArgumentIdLength + maxLongNameLength + 3; ++length) os << " ";

        // print the description
        os << "\t" << opt.description << std::endl;

    }

    // print the footer
    os << footer;
}

void GlobalConfig::error() {
    for(int i = 0; i < argc; i++) {
        std::cerr << argv[i] << " ";
    }
    std::cerr << "\nError parsing command-line arguments.\n";
    printOptions(std::cerr);
    exit(1);
}

};
