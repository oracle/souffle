#pragma once

#include "AstRelation.h"
#include "SouffleInterface.h"
#include "RamRelation.h"
#include "RamData.h"

#include "Logger.h"

namespace souffle {

class InterfaceResult {
public:
  InterfaceResult(SouffleProgram* prog)
    :p(prog), e(NULL), ty(RESULT_COMPILER)
  {}

  InterfaceResult(RamEnvironment* env)
    :p(NULL), e(env), ty(RESULT_INTERPRETER)
  {}

  ~InterfaceResult(){
     if (p != NULL){
       delete p;
     }
     if (e != NULL){
       delete e;
     }
   }

protected:

  enum resType{
    RESULT_INTERPRETER,
    RESULT_COMPILER
  };

public:
  std::vector<std::string> getRelationNames() {
    LOG(INFO) ENTERCPP("getRelationNames");
    std::vector<std::string> vec;

    LOG(INFO) PRE << "Program is " << p << "\n";
    assert(p != NULL);

    std::vector<Relation*> rels = p->getOutputRelations();
    LOG(INFO) PRE << "vec of relations from program is " << rels.size() << "\n";

    for (auto &r : rels){ 
      LOG(INFO) PRE << "relation is " << r << " of size " << r->size() << "\n";
      if(r == nullptr) {
        LOG(INFO) PRE << "relation is null : " << r << "\n";
        assert(false && "Relation is null");
      }
      
      LOG(INFO) PRE << "Adding relation name " << r->getName() << "\n";
      vec.push_back(r->getName());
    }
    
    LOG(INFO) LEAVECPP;
    return vec;
  }

  std::vector<PrimData*> getAllRelations() {
    LOG(INFO) ENTERCPP("getAllRelations");
    assert(ty != RESULT_INTERPRETER);

    std::vector<PrimData*> vec;
    std::vector<Relation*> outputrels = p->getOutputRelations();
    for(auto& v : outputrels) {
      vec.push_back(getRelationRowsCompile(v));
    }
    LOG(INFO) LEAVECPP;
    return vec; 
  }

  PrimData* getPrimRelation(std::string name) {
    LOG(INFO) ENTERCPP("getPrimRelation");
    if (ty == RESULT_INTERPRETER) {
      if(!e->hasRelation(name))
        return NULL;

      const RamRelation& res = e->getRelation(name);
      LOG(INFO) LEAVECPP;
      return getRelationRowsInterp(res);
    }

    assert(ty == RESULT_COMPILER);
    // Compiler
    Relation* res = p->getRelation(name);
    if(res == NULL){
      std::cout << "relation " << name << " not found!!!";
      return NULL;
    }
    LOG(INFO) LEAVECPP;
    return getRelationRowsCompile(res);
  }

private:

  PrimData* getRelationRowsInterp(const RamRelation& res) {
    std::vector<std::vector<std::string>> vec;
    res.store(vec, e->getSymbolTable(), res.getID().getSymbolMask());
    return new PrimData(vec);
  }

  PrimData* getRelationRowsCompile(Relation* res) {
    std::vector<std::vector<std::string>> vec;

    PrimData* primData = new PrimData();
    for(Relation::iterator it = res->begin(); it != res->end(); ++it) {
      std::vector<std::string> vecinner;
      tuple tu = (*it);
      for(size_t i = 0; i < res->getArity(); ++i){
        std::string val1;
        tu >> val1;
        vecinner.push_back(val1);
      }
      primData->data.push_back(vecinner);
    }
    return primData;
  }

private:
  SouffleProgram* p;
  RamEnvironment* e;
  resType ty;
};

}
