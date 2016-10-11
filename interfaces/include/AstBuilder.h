/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file ParserDriver.h
 *
 * Defines the parser driver.
 *
 ***********************************************************************/
#pragma once

#include <memory>
#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <stdarg.h>
#include <string>
#include <stack>
#include <unistd.h>

#include "Util.h"
#include "AstProgram.h"
#include "AstClause.h"
#include "AstComponent.h"
#include "AstRelation.h"
#include "AstArgument.h"
#include "AstTranslationUnit.h"
#include "AstNode.h"
#include "BinaryOperator.h"
#include "AstParserUtils.h"

#include "Logger.h"

namespace souffle {

class AstTranslationUnit;
class ErrorReport;
class AstRelation;
class AstType;
class AstProgram;
class SymbolTable;


class AstBuilder {
public:
    AstBuilder();
    virtual ~AstBuilder();

    AstTranslationUnit* translationUnit;

    bool trace_scanning;

    AstRelation* getRelation(std::string name);

    void addRelation(AstRelation *r);
    void addType(AstType *t);
    void addClause(AstClause *c);
    void addComponent(AstComponent *c);
    void addInstantiation(AstComponentInit *ci);
    std::string print();
    void compose(AstBuilder* other);

    AstTranslationUnit* getTranslationUnit() { 
      return translationUnit;
    }

    AstProgram* getProgram() { return translationUnit->getProgram(); }

    bool trace_parsing;

    souffle::SymbolTable &getSymbolTable();

    void error(const std::string &msg);
};

} // end of namespace souffle

