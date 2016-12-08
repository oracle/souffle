#pragma once


#include <string>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <memory>
#include <iomanip>


#include "Rule.hpp"

class Iteration {
private:
	double runtime = 0;
	long num_tuples = 0;
	double copy_time = 0;
	std::string locator = "";
	long prev_num_tuples = 0;

	std::unordered_map<std::string, std::shared_ptr<Rule>> rul_rec_map;

public:
	Iteration() {
		rul_rec_map = std::unordered_map<std::string, std::shared_ptr<Rule>>();
	}

	void addRule(std::vector<std::string> data, std::string rec_id);

	inline std::unordered_map<std::string, std::shared_ptr<Rule>> getRul_rec() { return this->rul_rec_map; }

	std::string toString();

	inline double getRuntime() { return runtime; }

	inline void setRuntime(double runtime) { this->runtime = runtime; }

	inline long getNum_tuples() { return num_tuples; }

	inline void setNum_tuples(long num_tuples) { this->num_tuples = num_tuples; }

	inline double getCopy_time() { return copy_time; }

	inline void setCopy_time(double copy_time) { this->copy_time = copy_time; }

	inline std::string getLocator() { return locator; }

	inline void setLocator(std::string locator) { this->locator = locator; }

};

