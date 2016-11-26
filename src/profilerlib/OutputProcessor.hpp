#pragma once

#include "ProgramRun.hpp"

class OutputProcessor {
private:
	ProgramRun programRun;
public:
	OutputProcessor() {
		programRun = ProgramRun();
	}

	ProgramRun getProgramRun() {
		return programRun;
	}

};