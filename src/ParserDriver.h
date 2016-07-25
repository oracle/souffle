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
#include "parser.hh"

#define YY_DECL yy::parser::symbol_type yylex(ParserDriver &driver, yyscan_t yyscanner)
YY_DECL;

namespace souffle {

class AstTranslationUnit;
class ErrorReport;
class AstRelation;
class AstType;
class AstProgram;
class SymbolTable;

typedef void* yyscan_t;

struct scanner_data {
    AstSrcLocation yylloc;

    /* Stack of parsed files */
    const char *yyfilename;

};

class ParserDriver {
public:
    ParserDriver();
    virtual ~ParserDriver();

    std::unique_ptr<AstTranslationUnit> translationUnit;

    void addRelation(AstRelation *r);
    void addType(AstType *t);
    void addClause(AstClause *c);
    void addComponent(AstComponent *c);
    void addInstantiation(AstComponentInit *ci);

    souffle::SymbolTable &getSymbolTable();

    bool trace_scanning;

    std::unique_ptr<AstTranslationUnit> parse(const std::string &f, FILE* in);
    std::unique_ptr<AstTranslationUnit> parse(const std::string &code);

    static std::unique_ptr<AstTranslationUnit> parseTranslationUnit(const std::string &f, FILE* in);
    static std::unique_ptr<AstTranslationUnit> parseTranslationUnit(const std::string &code);

    bool trace_parsing;

    void error(const AstSrcLocation &loc, const std::string &msg);
    void error(const std::string &msg);
};

} // end of namespace souffle

