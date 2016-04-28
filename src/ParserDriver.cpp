/*
 * Souffle version 0.0.0
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - souffle/LICENSE
 */

/************************************************************************
 *
 * @file ParserDriver.cpp
 *
 * Defines the parser driver.
 *
 ***********************************************************************/
#include "ParserDriver.h"
#include "AstTranslationUnit.h"
#include "ErrorReport.h"
#include "AstProgram.h"


typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern YY_BUFFER_STATE yy_scan_string(const char*, yyscan_t scanner);
extern int yylex_destroy(yyscan_t scanner);
extern int yylex_init_extra(scanner_data *data, yyscan_t *scanner);
extern void yyset_in(FILE *in_str, yyscan_t scanner);

namespace souffle {

ParserDriver::ParserDriver() : trace_scanning(false), trace_parsing(false) { }

ParserDriver::~ParserDriver() { }

std::unique_ptr<AstTranslationUnit> ParserDriver::parse(const std::string &f, FILE *in) {
    translationUnit = std::unique_ptr<AstTranslationUnit>(
            new AstTranslationUnit(std::unique_ptr<AstProgram>(new AstProgram())));
    yyscan_t scanner;
    scanner_data data;
    data.yyfilename = f.c_str();
    yylex_init_extra(&data, &scanner);
    yyset_in(in, scanner);

    yy::parser parser(*this, scanner);
    parser.set_debug_level(trace_parsing);
    parser.parse();

    yylex_destroy(scanner);

    translationUnit->getProgram()->finishParsing();

    return std::move(translationUnit);
}

std::unique_ptr<AstTranslationUnit> ParserDriver::parse(const std::string &code) {
    translationUnit = std::unique_ptr<AstTranslationUnit>(
            new AstTranslationUnit(std::unique_ptr<AstProgram>(new AstProgram())));

    scanner_data data;
    data.yyfilename = "<in-memory>";
    yyscan_t scanner;
    yylex_init_extra(&data, &scanner);
    yy_scan_string(code.c_str(), scanner);
    yy::parser parser(*this, scanner);
    parser.set_debug_level(trace_parsing);
    parser.parse();

    yylex_destroy(scanner);

    translationUnit->getProgram()->finishParsing();

    return std::move(translationUnit);
}

std::unique_ptr<AstTranslationUnit> ParserDriver::parseTranslationUnit(const std::string &f, FILE *in) {
    ParserDriver parser;
    return parser.parse(f, in);
}

std::unique_ptr<AstTranslationUnit> ParserDriver::parseTranslationUnit(const std::string &code) {
    ParserDriver parser;
    return parser.parse(code);
}

void ParserDriver::addRelation(AstRelation *r) {
    const auto& name = r->getName();
    if (AstRelation *prev = translationUnit->getProgram()->getRelation(name)) {
        Diagnostic err(Diagnostic::ERROR, DiagnosticMessage("Redefinition of relation " + toString(name), r->getSrcLoc()),
                {DiagnosticMessage("Previous definition", prev->getSrcLoc())});
        translationUnit->getErrorReport().addDiagnostic(err);
    } else {
        translationUnit->getProgram()->addRelation(std::unique_ptr<AstRelation>(r));
    }
}

void ParserDriver::addType(AstType *type) {
    if (const AstType *prev = translationUnit->getProgram()->getType(type->getName())) {
        Diagnostic err(Diagnostic::ERROR, DiagnosticMessage("Redefinition of type " + type->getName(), type->getSrcLoc()),
                {DiagnosticMessage("Previous definition", prev->getSrcLoc())});
        translationUnit->getErrorReport().addDiagnostic(err);
    } else {
        translationUnit->getProgram()->addType(std::unique_ptr<AstType>(type));
    }
}

void ParserDriver::addClause(AstClause *c) {
    translationUnit->getProgram()->addClause(std::unique_ptr<AstClause>(c));
}
void ParserDriver::addComponent(AstComponent *c) {
    translationUnit->getProgram()->addComponent(std::unique_ptr<AstComponent>(c));
}
void ParserDriver::addInstantiation(AstComponentInit *ci) {
    translationUnit->getProgram()->addInstantiation(std::unique_ptr<AstComponentInit>(ci));
}

souffle::SymbolTable &ParserDriver::getSymbolTable() {
    return translationUnit->getSymbolTable();
}

void ParserDriver::error(const AstSrcLocation &loc, const std::string &msg) {
    translationUnit->getErrorReport().addError(msg, loc);
}
void ParserDriver::error(const std::string &msg) {
    translationUnit->getErrorReport().addDiagnostic(Diagnostic(Diagnostic::ERROR, DiagnosticMessage(msg)));
}

} // end of namespace souffle

