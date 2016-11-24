#ifndef RULE_H
#define RULE_H

#include <string>
#include <sstream>

class Rule {

protected:
	std::string name;
	double runtime = 0;
	long num_tuples = 0;
	std::string identifier;
	std::string locator = "";

private:
	bool recursive = false;
	int version;

public:
	Rule() {}; // TODO: is this ok for setting variables in scope? or have to use pointers?

	Rule(std::string name, std::string id) 
	: name(name),
	  identifier(id) { }

	Rule(std::string name, int version, std::string id)
		: name(name),
		  identifier(id),
		  recursive(true),
		  version(version) { }
	

	std::string getId() {
		return identifier;
	}

	double getRuntime() {
		return runtime;
	}

	long getNum_tuples() {
		return num_tuples;
	}

	void setRuntime(double runtime) {
		this->runtime = runtime;
	}

	void setNum_tuples(long num_tuples) {
		this->num_tuples = num_tuples;
	}

	std::string getName() {
		return name;
	}

	void setId(std::string id) {
		identifier = id;
	}

	std::string getLocator() {
		return locator;
	}

	void setLocator(std::string locator) {
		if (this->locator.empty()) {
			this->locator = locator;
		} else {
			this->locator += " " + locator;
		}
	}

	bool isRecursive() {
		return recursive;
	}

	void setRecursive(bool recursive) {
		this->recursive = recursive;
	}

	int getVersion() {
		return version;
	}

	void setVersion(int version) {
		this->version = version;
	}

	std::string toString() {
		std::ostringstream output;
		if (recursive) {
			output << "{" << name << "," << version << ":";
		} else {
			output << "{" << name << ":";
		}
		output << "[" << runtime << "," << num_tuples << "]}";
		return output.str();
	}
};

#endif
