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
#include <iostream>
#include <assert.h>

namespace souffle {

class PrimData {

public:
  PrimData() {}

  PrimData(std::vector<std::vector<std::string> > d): data(d) {}

  std::vector<std::vector<std::string> > data;
};

class RamData {

  private:
    std::map<std::string, PrimData*> data_map;

  public:
    RamData() 
    {}

    RamData* merge(RamData* d) {
       RamData* nd = new RamData();
       nd->data_map.insert(data_map.begin(), data_map.end());
       nd->data_map.insert(d->data_map.begin(), d->data_map.end()); 
       return nd;
    }

    void addTuples(std::string name, PrimData* d) {
      assert(d != NULL);
      data_map[name] = d;
    }

    void addTuple(std::string name, std::vector<std::string> tuple) {
      if (data_map.find(name) == data_map.end()){
        data_map[name] = new PrimData();
        data_map[name]->data.push_back(tuple);
      }
      else {
        data_map[name]->data.push_back(tuple);
      }
    }

    std::map<std::string, PrimData*> getDataMap() {
      return data_map;
    }

    PrimData* getTuples(std::string name) {
      if (data_map.find(name) == data_map.end()) {
        std::cout << "name : " << name << " not in data_map\n";
        for(auto d : data_map){
            std::cout << " name = " << d.first << " \n";
            if(d.first == name) { 
              assert(false && "name is in data_map!!");
            }
        }
      }

      if(data_map[name]->data.size() == 0) {
          return NULL;
      }

      return data_map[name];
    }

    std::stringstream* getTuplesStr(std::string name) {
      std::stringstream* ss = new std::stringstream();
      std::vector<std::vector<std::string> > reldata = data_map[name]->data;

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
