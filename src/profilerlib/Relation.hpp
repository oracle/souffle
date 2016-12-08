
#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <memory>

#include "Rule.hpp"
#include "Iteration.hpp"


class Relation {
private:
    std::string name;
    double runtime = 0;
    long prev_num_tuples = 0;
    long num_tuples = 0;
    std::string id;
    std::string locator;
    int rul_id = 0;
    int rec_id = 0;

    std::vector<std::shared_ptr<Iteration>> iterations;

    std::unordered_map<std::string, std::shared_ptr<Rule>> ruleMap;

    bool ready = true;

public:

    Relation(std::string name, std::string id) :
    name(name),
    id(id) {
        ruleMap = std::unordered_map<std::string, std::shared_ptr<Rule>>();
        iterations = std::vector<std::shared_ptr<Iteration>>();
    }

    std::string createID() { return "N" + id.substr(1) + "." + std::to_string(++rul_id); }

    std::string createRecID(std::string name);

    inline double getNonRecTime() { return runtime; }

    double getRecTime();

    double getCopyTime();

    long getNum_tuplesRel();

    long getNum_tuplesRul();

    inline long getTotNum_tuples() { return getNum_tuplesRel(); }

    long getTotNumRec_tuples();

    inline void setRuntime(double runtime) { this->runtime = runtime; }

    inline void setNum_tuples(long num_tuples) { this->num_tuples = num_tuples; }


    std::string toString();

    inline std::string getName() { return name; }

    /**
     * @return the ruleMap
     */
    inline std::unordered_map<std::string, std::shared_ptr<Rule>>& getRuleMap() { return this->ruleMap; }

    std::vector<std::shared_ptr<Rule>> getRuleRecList();

    inline std::vector<std::shared_ptr<Iteration>>& getIterations() { return this->iterations; }
    
    inline std::string getId() { return id; }

    inline std::string getLocator() { return locator; }

    inline void setLocator(std::string locator) { this->locator = locator; }

	inline bool isReady() { return this->ready; }

	inline void setReady(bool ready) { this->ready = ready; }

	inline long getPrev_num_tuples() { return prev_num_tuples; }

	inline void setPrev_num_tuples(long prev_num_tuples) { this->prev_num_tuples = prev_num_tuples; }
};
