
#include <iostream>
#include <string>
#include <vector>

#include "OutputProcessor.hpp"
#include "Reader.hpp"

class Tui {
private:
	std::string f_name;
	bool alive = false;
	int sort_col = 0;
	int precision = -1;

	OutputProcessor out;

public:
	Tui(std::string filename, bool live) {
		out = OutputProcessor();
		ProgramRun run = out.getProgramRun();
//		std::cout << filename << std::endl;
		Reader read = Reader(filename, run, false, live);
    	read.readFile();

    	std::unordered_map<std::string, std::shared_ptr<Relation>> rel_map = read.retRelationMap();

//    	std::cout <<"\nReader Relation map:\n";
		std::cout << "{\n";
    	for (auto it = rel_map.begin(); it != rel_map.end(); ++it) {
    		std::cout <<  "\"" << it->first << "\":" << it->second->toString() << ",\n";
    	}
		std::cout << "}";

	}
};