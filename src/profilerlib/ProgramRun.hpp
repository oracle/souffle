#ifndef PROGRAMRUN_H
#define PROGRAMRUN_H

#include <unordered_map>
#include <string>
#include "Relation.hpp"

class ProgramRun {
private:
	long serialVersionUID = -4236231035756069616L; // TODO: change?
	double runtime;
	std::unordered_map<std::string, Relation> relation_map;
public:
	ProgramRun() {
		relation_map = std::unordered_map<std::string, Relation>();
		runtime = -1.0;
	}

	void SetRuntime(double runtime) {
		this->runtime = runtime;
	}

	void setRelation_map(std::unordered_map<std::string, Relation> relation_map) {
		this->relation_map = relation_map;
	}
};

#endif