#pragma once 

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cassert>
#include <exception>
#include <memory>
#include <regex>


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

//	static long filepointer;
	bool loaded = false;
	bool online;
	// Thread for live mode
//	std::string mes;

	double runtime;
	std::unordered_map<std::string, std::shared_ptr<Relation>> relation_map;
	int rel_id = 0;

public:
	std::shared_ptr<ProgramRun> run;

	Reader(std::string arg, std::shared_ptr<ProgramRun> run, bool vFlag, bool online) :
            file_loc(arg), run(run), file(std::ifstream(arg)), runtime(-1.0), online(online),
            relation_map(std::unordered_map<std::string,std::shared_ptr<Relation>>())
            { }

	/**
	 * Read the contents from file into the class
	 */
	void readFile();

	void save(std::string f_name);

	void process(const std::vector<std::string>& data);

	inline bool isLive() { return online; }

	void addIteration(std::shared_ptr<Relation> rel, std::vector<std::string> data);

	void addRule(std::shared_ptr<Relation> rel, std::vector<std::string> data);

	inline bool isLoaded() { return loaded; }
    std::string RelationcreateId() { return "R" + std::to_string(++rel_id); }

    std::string createId();


};
