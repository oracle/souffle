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
   // check number of arguments 
   if(argc != 2) error("wrong number of arguments!"); 

   // create instance of program "load_print"
   if(Program *prog = ProgramFactory::newInstance("load_print")) {

      // load all input relations from current directory
      prog->loadAll(argv[1]);
 
      // run program 
      prog->run(); 

      // print all relations to CSV files in current directory
      prog->printAll();

      // free program 
      delete prog; 

   } else {
      error("cannot find program load_print");        
   }
}
