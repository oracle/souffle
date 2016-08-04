/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2016, souffle-lang. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file RamData.h
 *
 * Define the class RamData to model data coming from external 
 * applications.
 *
 ***********************************************************************/
#pragma once

#include <string>
#include <vector>
#include <map>
#include <sstream>

namespace souffle {

class RamData {

  private:
    std::map<std::string, std::vector<std::vector<std::string> > > data_map;

  public:
    RamData() 
    {}

    void addTuple(std::string name, std::vector<std::string> tuple) {
      data_map[name].push_back(tuple);
    }

    std::map<std::string, std::vector<std::vector<std::string> > > getDataMap() {
      return data_map;
    }

    std::vector<std::vector<std::string> > getTuples(std::string name) {
      return data_map[name];
    }

    std::stringstream* getTuplesStr(std::string name) {
      std::stringstream* ss = new std::stringstream();
      std::vector<std::vector<std::string>> reldata = data_map[name];

      for(std::vector<std::string> vec : reldata) {
        for(std::string t : vec) {
         (*ss) << t << "\t";
        }
        (*ss) << "\n";
      }

      return ss;
    }
};

}
