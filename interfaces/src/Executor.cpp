#include "Executor.h"

using namespace souffle;


InterfaceResult* Executor::executeInterpreter(RamData* data) {
  LOG(INFO) ENTERCPP("executeInterpreter");

  RamInterpreter r;
  LOG(INFO) PRE << "About to run\n"; 
  RamEnvironment* e = r.execute(table, *rp, data);
  LOG(INFO) PRE << "Ran interpreter\n"; 

  LOG(INFO) LEAVECPP;
  return new InterfaceResult(e);
}


void Executor::compile(std::string& filename) {
  LOG(INFO) ENTERCPP("compile");
  std::string libname = "lib"+ filename + ".so";
  LOG(INFO) PRE << "Compiling to library\n";
  std::chrono::steady_clock::time_point compilebegin = std::chrono::steady_clock::now();
  RamCompiler r(filename);
  r.compileToLibrary(table, *rp, filename);
  std::chrono::steady_clock::time_point compileend = std::chrono::steady_clock::now();
  std::cout << "Compilation Duration = " << 
    std::chrono::duration_cast<std::chrono::microseconds>(compileend - compilebegin).count() <<
    std::endl;

  LOG(INFO) LEAVECPP;
}

InterfaceResult* Executor::executeCompiler(RamData* data, std::string& filename, bool comp) {
  LOG(INFO) ENTERCPP("executeCompile");
  //lambda to check if file exists
  auto existsfile = [](const std::string& name) -> bool {
    if (FILE *file = fopen(name.c_str(), "r")) {
      fclose(file);
      return true;
    } else {
      return false;
    }   
  };

  std::string libname = "lib"+ filename + ".so";
  if(!existsfile(libname.c_str()) && comp) {
    compile(filename);
  }

  void *lib_handle;
  SouffleProgram* (*fn)(const char* p);
  std::string error;
  std::string openfile = "./lib"+filename+".so";

  lib_handle = dlopen(openfile.c_str(), RTLD_LAZY);
  if (lib_handle == NULL){
    std::cout << "interface: Error! Cannot find library: " << openfile.c_str() << " ! \n";
    return NULL;
  }

  void* temp = dlsym(lib_handle, "getInstance");
  if(temp == NULL) {
    std::cout << "interface: Error! Cannot find symbol from lib\n";
    dlclose(lib_handle);
    return NULL;
  }

  memcpy(&fn, &temp, sizeof fn);

  SouffleProgram* p = (*fn)(filename.c_str());
  if(p == NULL) {
    std::cout << "interface: Program not found" << "\n";
    dlclose(lib_handle);
    assert(false && "program not found");
  }

  std::map<std::string, PrimData*> dmap = data->getDataMap();

  std::chrono::steady_clock::time_point sendbegin = std::chrono::steady_clock::now();

  for(auto& m : dmap) { 
    Relation* rel = p->getRelation(m.first);
    if (rel == nullptr) {
      LOG(WARN) PRE << "WARN: rel is null, cannot find: " << m.first << "\n"; 
      continue;
    }

    if (m.second->data.size() == 0) {
      LOG(WARN) PRE << "WARN: data is empty " << m.first << "\n"; 
      continue;
    }

    for(auto& row : m.second->data) {
      tuple t(rel);
      int i = 0;
      for(auto& r : row){
        if (*rel->getAttrType(i) == 'i') {
          if (r.find('X') != std::string::npos || r.find('x') != std::string::npos) {
            int32_t d = std::stoll(r.c_str(), NULL, 16);
            t << d;
          }
          else if (r.find('b') != std::string::npos) {
            int32_t d = std::stoll(r.c_str(), NULL, 2);
            t << d;
          }
          else{
            int32_t d = std::stoi(r.c_str(), NULL, 10);
            t << d;
          }
        }
        else {
          t << r;
        }
        ++i;
      }
      rel->insert(t);
    } 
  }
  std::chrono::steady_clock::time_point sendend = std::chrono::steady_clock::now();
  std::cout << "Data Load duration = " << std::chrono::duration_cast<std::chrono::microseconds>(sendend - sendbegin).count() << std::endl;

  std::chrono::steady_clock::time_point runbegin = std::chrono::steady_clock::now();
  LOG(INFO) PRE << "About to run\n"; 
  p->run();
  LOG(INFO) PRE << "Ran compiler\n"; 
  std::chrono::steady_clock::time_point runend = std::chrono::steady_clock::now();
  std::cout << "Run duration = " << std::chrono::duration_cast<std::chrono::microseconds>(runend - runbegin).count() << std::endl;
  //p->printAll();
  LOG(INFO) PRE << "Printed to output\n"; 
  //dlclose(lib_handle);
  LOG(INFO) LEAVECPP;
  return new InterfaceResult(p);
}
