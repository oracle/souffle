/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file AstBuilder.cpp
 *
 * Defines the parser driver.
 *
 ***********************************************************************/
#include "AstBuilder.h"
#include "AstTranslationUnit.h"
#include "ErrorReport.h"
#include "AstProgram.h"

namespace souffle {

AstBuilder::AstBuilder() : trace_scanning(false), trace_parsing(false) { 
    translationUnit = 
            new AstTranslationUnit(std::unique_ptr<AstProgram>(new AstProgram()), true);
}

AstBuilder::~AstBuilder() { }

AstRelation* AstBuilder::getRelation(std::string name) {
    return translationUnit->getProgram()->getRelation(name);
}

void AstBuilder::addRelation(AstRelation *r) {
    LOG(INFO) ENTERCPP("Add relation");
    const auto& name = r->getName();
    LOG(INFO) PRE << "Adding relation " << name << "\n";
    if (translationUnit->getProgram()->getRelation(name)) {
       LOG(INFO) PRE << "Relation " << name << " already exists\n";
       LOG(INFO) LEAVECPP;
       return;
    }

    translationUnit->getProgram()->addRelation(std::unique_ptr<AstRelation>(r));
    LOG(INFO) PRE << "Relation " << name << " added to progam\n";
    LOG(INFO) LEAVECPP;
}

void AstBuilder::compose(AstBuilder* other){
  //  AstProgram* p1 = translationUnit->getProgram();
  //  AstProgram* p2 = other->translationUnit->getProgram();

    //p1->types.insert(p2->types.begin(), p2->types.end());
    //p1->relations.insert(p2->relations.begin(), p2->relations.end());
//    p1->clauses.insert(p1->clauses.end(), p2->clauses.begin(), p2->clauses.end());
    //p1->components.insert( p1->components.end(), p2->components.begin(), p2->components.end());
    /*p1->instantiations.insert(
      p1->instantiaitions.end(), p2->instantiations.begin(), p2->instantiations.end()
    );*/
}

void AstBuilder::addType(AstType *type) {
    const auto& name = type->getName();
    if (translationUnit->getProgram()->getType(name)) {
      return;
    } else {
        translationUnit->getProgram()->addType(std::unique_ptr<AstType>(type));
    }
}

void AstBuilder::addClause(AstClause *c) {
    translationUnit->getProgram()->addClause(std::unique_ptr<AstClause>(c));
}

void AstBuilder::addComponent(AstComponent *c) {
    translationUnit->getProgram()->addComponent(std::unique_ptr<AstComponent>(c));
}

void AstBuilder::addInstantiation(AstComponentInit *ci) {
    translationUnit->getProgram()->addInstantiation(std::unique_ptr<AstComponentInit>(ci));
}

std::string AstBuilder::print() {
   std::stringstream ss;
   translationUnit->getProgram()->print(ss);
   return ss.str();
}

souffle::SymbolTable &AstBuilder::getSymbolTable() {
    return translationUnit->getSymbolTable();
}

void AstBuilder::error(const std::string &msg) {
    translationUnit->getErrorReport().addDiagnostic(Diagnostic(Diagnostic::ERROR, DiagnosticMessage(msg)));
}

} // end of namespace souffle

