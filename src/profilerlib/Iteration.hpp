#ifndef ITERATION_HPP
#define ITERATION_HPP


#include <string>
#include <unordered_map>
#include <cassert>

#include "Rule.hpp"

class Iteration {
private:
	double runtime = 0;
	long num_tuples = 0;
	double copy_time = 0;
	std::string locator = "";
	long prev_num_tuples = 0;

	std::unordered_map<std::string, Rule> rul_rec_map;
public:
	Iteration() {
		rul_rec_map = std::unordered_map<std::string, Rule>();
	}

	void addRule(std::vector<std::string> data, std::string rec_id) {
		std::string strTemp = data[4] + data[3] + data[2];
		if (data[0].at(0) == 't') {
			// TODO important: ensure same result as java
			std::unordered_map<std::string,Rule>::const_iterator got = rul_rec_map.find(strTemp);
			if (got!=rul_rec_map.end()) {
				Rule rul_rec = got->second;
				rul_rec.setRuntime(std::stod(data[5])+
					rul_rec.getRuntime());
			} else {
				Rule rul_rec = Rule(data[4],
					std::stoi(data[2]), rec_id);
				rul_rec.setRuntime(std::stod(data[5]));
				rul_rec.setLocator(data[3]);
				rul_rec_map.insert({strTemp, rul_rec});
			}
		} else if (data[0].at(0) == 'n') {
			Rule rul_rec = rul_rec_map.find(strTemp)->second;
			//assert (rul_rec != rul_rec_map.end() && "missing t tag");
			rul_rec.setNum_tuples(std::stol(data[5]) - prev_num_tuples);
			prev_num_tuples = std::stol(data[5]);
			rul_rec_map.insert({strTemp, rul_rec});
		}
	}

	std::unordered_map<std::string, Rule> getRul_rec() {
		return rul_rec_map;
	}

	std::string toString() {
		std::ostringstream output;
		// dont think "" is necessary, but left it to make it the same as java code
		output << "" << runtime << "," << num_tuples << "," << copy_time << ",";
		output << " recRule:";
		for (auto &rul : rul_rec_map)
    		output << rul.second.toString();
    	output << "\n";
    	return output.str();
	}

	double getRuntime() {
		return runtime;
	}

	void setRuntime(double runtime) {
		this->runtime = runtime;
	}

	long getNum_tuples() {
		return num_tuples;
	}

	void setNum_tuples(long num_tuples) {
		this->num_tuples = num_tuples;
	}

	double getCopy_time() {
		return copy_time;
	}

	void setCopy_time(double copy_time) {
		this->copy_time = copy_time;
	}

	std::string getLocator() {
		return locator;
	}

	void setLocator(std::string locator) {
		this->locator = locator;
	}

};

#endif