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
 * @file AstTranslationUnit.h
 *
 * Define a class that represents a Datalog translation unit, consisting
 * of a datalog program, error reports and cached analysis results.
 *
 ***********************************************************************/
#pragma once

#include <map>
#include <memory>

#include "AstAnalysis.h"
#include "AstProgram.h"
#include "ErrorReport.h"
#include "DebugReport.h"
#include "SymbolTable.h"

namespace souffle {

class AstTranslationUnit {
private:
    mutable std::map<std::string, std::unique_ptr<AstAnalysis> > analyses;

    /* Program AST */
    std::unique_ptr<AstProgram> program;

    /* The table of symbols encountered in the input program */
    souffle::SymbolTable symbolTable;

    ErrorReport errorReport;

    DebugReport debugReport;


public:
    AstTranslationUnit(std::unique_ptr<AstProgram> program) : program(std::move(program)) { }

    virtual ~AstTranslationUnit() { }

    template <class Analysis>
    Analysis *getAnalysis() const {
        std::string name = Analysis::name;
        auto it = analyses.find(name);
        if (it == analyses.end()) {
            // analysis does not exist yet, create instance and run it.
            analyses[name] = std::unique_ptr<AstAnalysis>(new Analysis());
            analyses[name]->run(*this);
        }
        return dynamic_cast<Analysis *>(analyses[name].get());
    }

    AstProgram *getProgram() {
        return program.get();
    }

    const AstProgram *getProgram() const {
        return program.get();
    }

    souffle::SymbolTable &getSymbolTable() {
        return symbolTable;
    }

    const souffle::SymbolTable &getSymbolTable() const {
        return symbolTable;
    }

    ErrorReport &getErrorReport() {
        return errorReport;
    }

    const ErrorReport &getErrorReport() const {
        return errorReport;
    }

    void invalidateAnalyses() {
        analyses.clear();
    }

    DebugReport &getDebugReport() {
        return debugReport;
    }

    const DebugReport &getDebugReport() const {
        return debugReport;
    }
};

} // end of namespace souffle

