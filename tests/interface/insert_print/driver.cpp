/*
 * Copyright (c) 2015, Oracle and/or its affiliates.
 *
 * All rights reserved.
 */

/************************************************************************
 *
 * @file driver.cpp 
 *
 * Driver program for invoking a Souffle program using the OO-interface
 *
 ***********************************************************************/

#include "souffle/SouffleInterface.h"

using namespace souffle; 

/**
 * Declare static vars for program factory 
 */
ProgramFactory *ProgramFactory::base = nullptr;

/**
 * Error handler
 */ 
void error(std::string txt) 
{
   std::cerr << "error: " << txt << "\n"; 
   exit(1); 
} 

/**
 * Main program
 */
int main(int argc, char **argv)
{
   // create an instance of program "insert_print"
   if(Program *prog = ProgramFactory::newInstance("insert_print")) {
      // get input relation "edge" 
      if(Relation *edge = prog->getRelation("edge")) {
         std::vector<std::array<std::string,2>> myData={
            {"A","B"}, 
            {"B","C"}, 
            {"C","D"}, 
            {"D","E"},
            {"E","F"},
            {"F","A"}
         }; 
         for(auto input : myData) { 
            tuple t(edge);
            t << input[0] << input[1];
            edge->insert(t); 
         }

         // run program 
         prog->run(); 

         // print all relations to CSV files in current directory
         // NB: Defaul is current directory
         prog->printAll();

         // free program analysis
         delete prog; 

      } else {
         error("cannot find relation edge");        
      }
   } else {
      error("cannot find program insert_print");        
   }
}
