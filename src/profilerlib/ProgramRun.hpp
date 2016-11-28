#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include "Relation.hpp"

class ProgramRun {
private:
	std::unordered_map<std::string, std::shared_ptr<Relation>> relation_map;
    int rel_id = 0;
    double runtime;
    double tot_rec_tup = 0.0;
    double tot_copy_time = 0.0;
public:
	ProgramRun() : relation_map(),
                   runtime(-1.0){}

	inline void SetRuntime(double runtime) { this->runtime = runtime; }

	inline void setRelation_map(std::unordered_map<std::string, std::shared_ptr<Relation>>& relation_map) {
		this->relation_map = relation_map;
	}

//    inline void update() {
//        //TODO: is (double) ok?
//        tot_rec_tup = (double)getTotNumRecTuples();
//        tot_copy_time = getTotCopyTime();
//    };

    std::string toString();
};
