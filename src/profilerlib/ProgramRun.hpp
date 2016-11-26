#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include "Relation.hpp"

class ProgramRun {
private:
	double runtime = -1.0;
	std::unordered_map<std::string, std::shared_ptr<Relation>> relation_map;
public:
	ProgramRun() : relation_map() {}

	inline void SetRuntime(double runtime) { this->runtime = runtime; }

	void setRelation_map(std::unordered_map<std::string, std::shared_ptr<Relation>>& relation_map) {
		this->relation_map = relation_map;
	}
};
