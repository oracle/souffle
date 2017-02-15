/************************************************************************
 *
 * @file Executor.h
 *
 * Executor for Souffle
 *
 ***********************************************************************/
#pragma once

#include <stdlib.h>
#include <errno.h>
#include <libgen.h>
#include <limits.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/stat.h>
#include <getopt.h>
#include <config.h>

#include <chrono>
#include <list>
#include <iostream>
#include <fstream>

#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <assert.h>

#include "Util.h"
#include "RamExecutor.h"
#include "RamData.h"
#include "RamStatement.h"

#include "SymbolTable.h"
#include "SouffleInterface.h"
#include "InterfaceResult.h"

#include "Logger.h"

// Shared Library loading
#include <dlfcn.h>


namespace souffle {

class Executor {
public:
  Executor(SymbolTable symb, std::unique_ptr<RamStatement> ram) : 
    table(symb), rp(std::move(ram)) 
  {}

  InterfaceResult* executeInterpreter(RamData* data);
  void compile(std::string& filename);
  InterfaceResult* executeCompiler(RamData* data, std::string& filename, bool comp = true);
private:
  SymbolTable table;
  std::unique_ptr<RamStatement> rp;
};

}

