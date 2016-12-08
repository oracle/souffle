#pragma once 

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cassert>
#include <exception>
#include <memory>
#include <regex>

#include "Relation.hpp"
#include "ProgramRun.hpp"
#include "Rule.hpp"
#include "Iteration.hpp"
#include "StringUtils.hpp"

class Reader {
private:
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
            run(run), file(std::ifstream(arg)), online(online),
            relation_map(std::unordered_map<std::string,std::shared_ptr<Relation>>()),
            runtime(-1.0){ }

	/**
	 * Read the contents from file into the class
	 */
	void readFile();

	/**
	 * TODO: write this docs
	 * @param data
	 */
	void process(const std::vector<std::string>& data);

	inline bool isLive() { return online; }

	/**
	 * TODO: write docs
	 * @param rel
	 * @param data
	 */
	void addIteration(std::shared_ptr<Relation> rel, std::vector<std::string> data);

	void addRule(std::shared_ptr<Relation> rel, std::vector<std::string> data);

	inline bool isLoaded() { return loaded; }


    /**
     * temporary: for testing purposes
     */
	inline std::unordered_map<std::string, std::shared_ptr<Relation>> retRelationMap() { return this->relation_map; }



//	std::vector<std::string> replace(std::string str);

    std::string RelationcreateId() { return "R" + std::to_string(++rel_id); }

    std::string createId();
};
