
#include "Cli.hpp"


static bool DEBUG=false;



void Cli::error() {
    //std::cout << "\nExpected commands/args to run souffle-prof: \n";
    std::cout << "./souffle-profile [-f|-j <file> [-c <command>] [-l]] [-h]\n";
    exit(1);
}

void Cli::parse() {
    std::vector<std::string> commands = std::vector<std::string>();
    std::string filename;
    bool alive = false;

    if (args.size() <= 1) {
        error();
    }

    for (int i=1; i<args.size(); i++) {
        std::string arg = args.at(i++);
        if (arg.at(0) != '-') {
            error();
        }

        if (arg.compare("-h")==0) {
            std::cout << "Souffle Profiler Alpha 4 (14/12/16)\n";
            error();
        } else if (arg.compare("-c")==0) {
            if (i < args.size()) {
                // split at \s+ and save into commands
                std::string& command_str = args.at(i++);
                commands = Tools::split(command_str, "\\s+");

            } else {
                std::cout << "Parameters for option -c missing!\n";
                error();
            }
        } else if (arg.compare("-f")==0) {
            if (i < args.size()) {
                filename = args.at(i);
            } else {
                std::cout << "Parameters for option -f missing!\n";
                error();
            }
        } else if (arg.compare("-j")==0) {
            if (i < args.size()) {
                filename = args.at(i);
            } else {
                std::cout << "Parameters for option -j missing!\n";
                error();
            }
        } else if (arg.compare("-l")==0) {
            alive = true;
        } else {
            std::cout << "Unknown argument " << args.at(i) << "\n";
            error();
        }
    }


    if (DEBUG) {
    std::cout << "\n-----\nstarting profiler with file: " << filename << "\n";
        if (alive) {
            std::cout << "alive: true\n";
        } else {
            std::cout << "alive: false\n";
        }
    }

    if (commands.size() > 0) {
        if (DEBUG) {
            std::cout << "commands: true\n";

            for (int j=0; j<commands.size();j++) {
                std::string& command = commands.at(j);
                std::cout << "command: " << commands.at(j) << "\n";
            }
        }
        Tui(filename, alive).runCommand(commands);

    } else {
        if (DEBUG)
            std::cout << "commands: false\n";
        if (args.at(1).compare("-j") == 0) {
            if (DEBUG)
                std::cout << "json output: true\n";

            Tui(filename, alive).runProf();
        } else {
            if (DEBUG)
                std::cout << "json output: false\n";

            Tui(filename, alive).runProf();
        }
    }
}
