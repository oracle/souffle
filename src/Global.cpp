#include "Global.h"

namespace souffle {


void MainConfig::processArgs(int argc, char** argv, const std::string header, const std::string footer, const std::vector<MainOption> mainOptions) {

    // START STAGE 1
    {
        // print the header
        ss << header;

        // iterate over the options and obtain the maximum name and argument lengths
        int maxLongNameLength = 0, maxArgumentIdLength = 0;
        for (const MainOption& opt : mainOptions) {
            // if it is the main option, do nothing
            if (opt.longName == "") continue;
            // otherwise, proceed with the calculation
            maxLongNameLength = ((int) opt.longName.size() > maxLongNameLength) ? opt.longName.size() : maxLongNameLength;
            maxArgumentIdLength = ((int) opt.argument.size() > maxArgumentIdLength) ? opt.argument.size() : maxArgumentIdLength;
        }

        // iterator over the options and pretty print them, using padding as determined
        // by the maximum name and argument lengths
        int length;
        for (const MainOption& opt : mainOptions) {

            // if it is the main option, do nothing
            if (opt.longName == "") continue;

            // print the short form name and the argument parameter
            length = 0;
            ss << "\t";
            if (isalpha(opt.shortName)) {
                ss << "-" << opt.shortName;
                if (!opt.argument.empty()) {
                    ss << "<" << opt.argument << ">";
                    length = opt.argument.size() + 2;
                }
            } else {
                ss << "  ";
            }

            // pad with empty space for prettiness
            for (; length < maxArgumentIdLength + 2; ++length) ss << " ";

            // print the long form name and the argument parameter
            length = 0;
            ss << "\t" << "--" << opt.longName;
            if (!opt.argument.empty()) {
                ss << "=<" << opt.argument << ">";
                length = opt.argument.size() + 3;
            }

            // again, pad with empty space for prettiness
            for (length += opt.longName.size(); length < maxArgumentIdLength + maxLongNameLength + 3; ++length) ss << " ";

            // print the description
            ss << "\t" << opt.description << std::endl;

        }

        // print the footer
        ss << footer;

        helpText = ss.str();
    } // END STAGE 1

    // START STAGE 2
    {

        option longNames[mainOptions.size()];
        std::string shortNames = "";
        std::map<const char, const MainOption*> optionTable;
        int i = 0;
        for (const MainOption& opt : mainOptions) {
            assert(opt.shortName != '?' && "short name for option cannot be '?'");
            optionTable[opt.shortName] = &opt;
            if (!opt.byDefault.empty())
                set(opt.longName, opt.byDefault);
            if (opt.longName == "")
                continue;
            longNames[i] = (option) { opt.longName.c_str(), (!opt.argument.empty()), nullptr, opt.shortName };
            shortNames += opt.shortName;
            if (!opt.argument.empty())
                shortNames += ":";
            ++i;
        }
        longNames[i] = {nullptr, false, nullptr, 0}; // the terminal option, needs to be null

        int c;     /* command-line arguments processing */
        while ((c = getopt_long(argc, argv, shortNames.c_str(), longNames, nullptr)) != EOF) {
            if (c == '?')
                Error::error("unexpected command line argument", []() { Global::config().printHelp(std::cerr); });
            auto iter = optionTable.find(c);
            if (iter == optionTable.end())
                assert("unexpected case in getopt");
            std::string arg = (optarg) ? std::string(optarg) : std::string();
            if (!iter->second->delimiter.empty())
                set(iter->second->longName, get(iter->second->longName) + iter->second->delimiter + arg);
            else
                set(iter->second->longName, arg);
        }

        /* Get the name of the datalog file. */
        std::string filenames = "";
        if (optind < argc) {
            for (; optind < argc; optind++) {
                if (!existFile(argv[optind])) {
                    Error::error("error: cannot open file " + std::string(argv[optind]));
                }
                if (filenames == "") {
                    filenames = argv[optind];
                } else {
                    filenames = filenames + " " + std::string(argv[optind]);
                }
            }
        } else {
            if (c == '?') Error::error("unexpected command line argument", []() { Global::config().printHelp(std::cerr); });
        }
        set("", filenames);
    }
    // END STAGE 2
}



};
