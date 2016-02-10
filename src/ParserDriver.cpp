/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All Rights reserved
 * 
 * The Universal Permissive License (UPL), Version 1.0
 * 
 * Subject to the condition set forth below, permission is hereby granted to any person obtaining a copy of this software,
 * associated documentation and/or data (collectively the "Software"), free of charge and under any and all copyright rights in the 
 * Software, and any and all patent rights owned or freely licensable by each licensor hereunder covering either (i) the unmodified 
 * Software as contributed to or provided by such licensor, or (ii) the Larger Works (as defined below), to deal in both
 * 
 * (a) the Software, and
 * (b) any piece of software and/or hardware listed in the lrgrwrks.txt file if one is included with the Software (each a “Larger
 * Work” to which the Software is contributed by such licensors),
 * 
 * without restriction, including without limitation the rights to copy, create derivative works of, display, perform, and 
 * distribute the Software and make, use, sell, offer for sale, import, export, have made, and have sold the Software and the 
 * Larger Work(s), and to sublicense the foregoing rights on either these or other terms.
 * 
 * This license is subject to the following condition:
 * The above copyright notice and either this complete permission notice or at a minimum a reference to the UPL must be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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

