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
 * @file ParserDriver.h
 *
 * Defines the parser driver.
 *
 ***********************************************************************/
#pragma once

#include <memory>
#include "parser.hh"

class AstTranslationUnit;
class ErrorReport;
class AstRelation;
class AstType;
class AstProgram;

namespace souffle {
class SymbolTable;
}
typedef void* yyscan_t;
#define YY_DECL yy::parser::symbol_type yylex(ParserDriver &driver, yyscan_t yyscanner)

YY_DECL;

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
