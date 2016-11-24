
#ifndef RELATION_H
#define RELATION_H

#include <vector>
#include <unordered_map>
#include <string>

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

    std::vector<Iteration> iterations;
    std::unordered_map<std::string, Rule> ruleMap;

    bool ready = true;

public:
    Relation(std::string name, std::string id) :
    name(name),
    id(id) {
        ruleMap = std::unordered_map<std::string, Rule>();
        iterations = std::vector<Iteration>();
    }

    std::string createID() {
        rul_id++;
        return "N" + id.substr(1) + "." + std::to_string(rec_id);
    }

    std::string createRecID(std::string name) {
        for (auto &iter : iterations) {
            for (auto &rul : iter.getRul_rec()) {
                if (rul.second.getName().compare(name)==0) {
                    return rul.second.getId();
                }
            }
        }
        rec_id++;
        return "C" + id.substr(1) + "." + std::to_string(rec_id);
    }

    double getNonRecTime() {
        return runtime;
    }

    double getRecTime() {
        double result = 0;
        for (auto &iter : iterations) {
            result += iter.getRuntime();
        }
        return result;
    }

    double getCopyTime() {
        double result = 0;
        for (auto &iter : iterations) {
            result += iter.getCopy_time();
        }
        return result;
    }

    long getNum_tuplesRel() {
        long result = 0L;
        for (auto &iter : iterations) {
            result += iter.getNum_tuples();
        }
        return num_tuples + result;
    }

    long getNum_tuplesRul() {
        long result = 0L;
        for (auto &rul : ruleMap) {
            result += rul.second.getNum_tuples();
        }
        for (auto &iter : iterations) {
            for (auto &rul : iter.getRul_rec()) {
                result += rul.second.getNum_tuples();
            }
        }
        return result;
    }

    long getTotNum_tuples() {
        return getNum_tuplesRel();
    }

    long getTotNumRec_tuples() {
        long result = 0L;
        for (auto &iter : iterations) {
            for (auto &rul : iter.getRul_rec()) {
                result += rul.second.getNum_tuples();
            }
        }
        return result;
    }

    void setRuntime(double runtime) {
        this->runtime = runtime;
    }

    void setNum_tuples(long num_tuples) {
        this->num_tuples = num_tuples;
    }

    
    std::string toString() {
        std::ostringstream output;
        output << "{\n" << name << ":" << runtime << ";" << num_tuples
                << "\n\nonRecRules:\n";
        for (auto &rul : ruleMap) {
            output << rul.second.toString();
        }
        output << "\n\niterations:\n";
        output << "[";
        for (auto &iter : iterations) {
        	output << iter.toString();
        	output << " ,";
    	} // TODO: remove last comma
    	output << "]";
        output << "\n}";
        return output.str();
    }

    std::string getName() {
        return name;
    }

    /**
     * @return the ruleMap
     */
    std::unordered_map<std::string, Rule> getRuleMap() {
        return ruleMap;
    }

    std::vector<Rule> getRuleRecList() {
        std::vector<Rule> temp = std::vector<Rule>();
        for (auto &iter : iterations) {
            for (auto &rul : iter.getRul_rec()) {
                temp.push_back(rul.second);
            }
        }
        return temp;
    }

    std::vector<Iteration> getIterations() {
        return iterations;
    }

    std::string getId() {
        return id;
    }

    std::string getLocator() {
        return locator;
    }

    void setLocator(std::string locator) {
        this->locator = locator;
    }

	bool isReady() {
		return ready;
	}

	void setReady(bool ready) {
		this->ready = ready;
	}

	long getPrev_num_tuples() {
		return prev_num_tuples;
	}

	void setPrev_num_tuples(long prev_num_tuples) {
		this->prev_num_tuples = prev_num_tuples;
	}
};
#endif