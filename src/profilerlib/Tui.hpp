
#include <iostream>
#include <string>
#include <vector>

#include "OutputProcessor.hpp"

class Tui {
private:
	std::string f_name;
	bool alive = false;
	int sort_col = 0;
	int precision = -1;

	OutputProcessor out;

public:
	Tui(std::string filename, bool live) {
		out = OutputProcessor();
		ProgramRun run = out.getProgramRun();


	}
};