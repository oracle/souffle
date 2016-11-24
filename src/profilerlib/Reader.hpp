#pragma once 

#include <iostream>
#include <fstream>
#include <unordered_map>
#include <cassert>

#include "Relation.hpp"
#include "ProgramRun.hpp"
#include "Rule.hpp"
#include "Iteration.hpp"

class Reader {
private:
	std::ifstream file;

	static long filepointer;
	bool loaded = false;
	bool online;
	// Thread for live mode
	std::string mes;

	double runtime;
	std::unordered_map<std::string, Relation> relation_map;
	int rel_id = 0;



	std::vector<std::string> splitOnStr(std::string str, std::string split) {
		std::vector<std::string> out = std::vector<std::string>();

		size_t i = 0;
		size_t last_i = 0;
		bool breakout = false;
		while((i=str.find(split, i+1))) {
			if (i==std::string::npos) {
				i = str.size();
				breakout = true;
			}
			std::string temp_part = "";
			
			// // ignore starting whitespace
			// while (str.at(last_i) == ' ' || str.at(last_i) == '\t') {
			// 	last_i++;
			// }

			for (last_i++;last_i<i;last_i++) {
				// // ignore trailing whitespace
				// size_t temp_last_i = last_i;
				// while (str.at(temp_last_i) == ' ' || str.at(temp_last_i) == '\t') {
				// 	temp_last_i++;
				// 	if (temp_last_i >= i) {
				// 		break;
				// 	}
				// }
				// if (temp_last_i == i) {
				// 	last_i = i;
				// } else {
					temp_part += str.at(last_i);
				// }
			}

			out.push_back(temp_part);
			if (breakout) {
				break;
			}
		}
		return out;

	}


	std::vector<std::string> replace(std::string str) {
		// ignore semicolons from strings, then cut where remaining semicolons are
		// then insert semicolons back
		bool ignore = false;
		std::string temp1 = "";
		if (str.find("\'")!=std::string::npos) {
			for (auto &c : str) {
				if (c == '\'') {
					ignore = !ignore;
				}
				if (ignore && c==';') {
					temp1 += '\b';
				} else {
					temp1 += c;
				}
			}
		} else {
			temp1 = str;
		}
		ignore = false;
		std::string temp2 = "";
		if (temp1.find("\"")!=std::string::npos) {
			for (auto &c : temp1) {
				if (c == '\"') {
					ignore = !ignore;
				}
				if (ignore && c==';') {
					temp2 += '\t';
				} else {
					temp2 += c;
				}
			}
		} else {
			temp2 = temp1;
		}
		temp2 = temp2.substr(1);
		std::vector<std::string> part = splitOnStr(temp2, ";");
		// go through each string and put semicolons back in where needed
		for (auto& i : part) {
			for (auto& c : i) {
				if (c == '\b' || c == '\t') {
					c = ';';
				}
			}
		}
		return part;
	}

	std::string createId() {
		return "R" + std::to_string(++rel_id);
	}

public:
	ProgramRun run;

	Reader(std::string arg, ProgramRun run, bool vFlag, bool online) {
		this->run = run;
		file = std::ifstream(arg);
		this->online = online;
		relation_map = std::unordered_map<std::string, Relation>();
		runtime = -1.0;
	}

	void readFile() {
		if (isLive()) {
			std::cout << "No live support";
		} else {
			if (file.is_open() == false) {
			  // error
			}

			std::string str;
			while(getline(file, str) ) {  
				std::cout << str << "\n";
			  	if(!str.empty() && str.at(0) == '@') {
			  		if (str.compare("@start-debug")==0) {
			  			continue;
			  		}
			  		std::vector<std::string> part;
			  		if (str.find("\'")!=std::string::npos ||
			  				str.find("\"")!=std::string::npos) {
			  			part = replace(str);
			  		} else {
			  			part = splitOnStr(str, ";");

			  		}
		  			// std::cout << "\nLINE SPLIT: [(";
		  			// for (auto& i : part) {
		  			// 	std::cout << i << "),(";
		  			// }
		  			// std::cout << ")]\n";

		  			process(part);
			  	}
			}
			file.close();
			loaded = true;
		}
	}


	void process(std::vector<std::string> data) {
		Relation _rel;

		if (data[0].compare("runtime")==0) {
			runtime = stod(data[1]);
		} else {
			if (relation_map.find(data[1])==relation_map.end()) {
				_rel = Relation(data[1], createId());
				relation_map.insert({data[1], _rel});
			} else {
				_rel = relation_map.find(data[1])->second;
			}
			// find nonrecursive first, since they both share recursive :)
			if (data[0].find("nonrecursive")!=std::string::npos) {
				if (data[0].at(0) == 't' && data[0].find("relation")!=std::string::npos) {
					_rel.setRuntime(std::stod(data[3]));
					_rel.setLocator(data[2]);
				} else if (data[0].at(0) == 'n' && data[0].find("relation")!=std::string::npos) {
					_rel.setNum_tuples(std::stol(data[3]));
				} else if (data[0].find("rule")!=std::string::npos) {
					addRule(_rel, data);
				}
			} else if (data[0].find("recursive")!=std::string::npos){
				addIteration(_rel, data);
			}
		}

		run.SetRuntime(this->runtime);
		run.setRelation_map(this->relation_map);
	}

	bool isLive() {
		return online;
	}

	void addIteration(Relation rel, std::vector<std::string> data) {
		bool ready = rel.isReady();
		std::vector<Iteration> iterations = rel.getIterations();
		std::string locator = rel.getLocator();

		Iteration iter;

		if (ready || iterations.empty()) {
			iter = Iteration();
			iterations.push_back(iter);
			rel.setReady(false);
		} else {
			iter = iterations.back();
		}

		if (data[0].find("rule") != std::string::npos) {
			std::string temp = rel.createRecID(data[4]);
			iter.addRule(data, temp);
		} else if (data[0].at(0)=='t'&& data[0].find("relation")!= std::string::npos) {
			iter.setRuntime(std::stod(data[3]));
			iter.setLocator(data[2]);
			rel.setLocator(data[2]);
		} else if (data[0].at(0)=='n'&& data[0].find("relation")!= std::string::npos) {
			iter.setNum_tuples(std::stol(data[3]));
		} else if (data[0].at(0)=='c'&& data[0].find("relation")!= std::string::npos) {
			iter.setCopy_time(std::stod(data[3]));
			rel.setReady(true);
		}

	}

	void addRule(Relation rel, std::vector<std::string> data) {
		Rule _rul;

		std::unordered_map<std::string,Rule> ruleMap = rel.getRuleMap();
		long prev_num_tuples = rel.getPrev_num_tuples();


		if (ruleMap.find(data[3]) == ruleMap.end()) {
			_rul = Rule(data[3], rel.createID());
			ruleMap.insert({data[3], _rul});
		} else {
			_rul = ruleMap.find(data[3])->second;
		}

		if (data[0].at(0) == 't') {
			_rul.setRuntime(std::stod(data[4]));
			_rul.setLocator(data[2]);
		} else if (data[0].at(0) == 'n') {
			// assert(rul);
			_rul.setNum_tuples(std::stol(data[4]) - prev_num_tuples);
			rel.setPrev_num_tuples(std::stol(data[4]));
		}
	}

	bool isLoaded() {
		return loaded;
	}
};
