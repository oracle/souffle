#pragma once

#include<string>
#include<vector>
#include<memory>

#include "ProgramRun.hpp"
#include "StringUtils.hpp"
#include "Table.hpp"
#include "Row.hpp"
#include "Cell.hpp"

class OutputProcessor {
private:
	std::shared_ptr<ProgramRun> programRun;
public:
	OutputProcessor() {
		programRun = std::make_shared<ProgramRun>(ProgramRun());
	}

	inline std::shared_ptr<ProgramRun>& getProgramRun() { return programRun; }

//
    Table getRelTable();
//    //Table getRulTableGui();
    Table getRulTable();
//    //Table getVersionsGui(String strRel, String strRul);
    Table getVersions(std::string strRel, std::string strRul);


    std::string formatTime(double number) {return Tools::formatTime(number);}
    std::string formatNum(int precision, long number) {return Tools::formatNum(precision, number);}
    std::vector<std::vector<std::string>> formatTable(Table table, int precision) {return Tools::formatTable(table, precision);}

};