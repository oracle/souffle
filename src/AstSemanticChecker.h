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
 * @file AstSemanticChecker.h
 *
 * Defines the semantic checker pass.
 *
 ***********************************************************************/
#pragma once
#include "AstTransformer.h"

class AstTranslationUnit;
class ErrorReport;
class AstProgram;
class AstAtom;
class AstLiteral;
class AstAggregator;
class AstArgument;
class AstClause;
class AstRelation;
class AstComponent;
class AstComponentScope;
class AstComponentType;
class AstComponentInit;
struct AstSrcLocation;
class TypeBinding;
class AstUnionType;
struct AstRecordType;
class AstType;
class TypeEnvironment;
class TypeAnalysis;
class ComponentLookup;
class PrecedenceGraph;

class AstSemanticChecker : public AstTransformer {
private:
    virtual bool transform(AstTranslationUnit &translationUnit);

    static void checkProgram(ErrorReport &report, const AstProgram &program, const TypeEnvironment &typeEnv, const TypeAnalysis &typeAnalysis, const ComponentLookup &componentLookup, const PrecedenceGraph &precedenceGraph);

    static void checkAtom(ErrorReport& report, const AstProgram& program, const AstAtom& atom);
    static void checkLiteral(ErrorReport& report, const AstProgram& program, const AstLiteral& literal);
    static void checkAggregator(ErrorReport& report, const AstProgram& program, const AstAggregator& aggregator);
    static void checkArgument(ErrorReport& report, const AstProgram& program, const AstArgument& arg);
    static void checkConstant(ErrorReport& report, const AstArgument& argument);
    static void checkFact(ErrorReport& report, const AstProgram& program, const AstClause& fact);
    static void checkClause(ErrorReport& report, const AstProgram& program, const AstClause& clause);
    static void checkRelationDeclaration(ErrorReport& report, const TypeEnvironment &typeEnv, const AstProgram& program, const AstRelation& relation);
    static void checkRelation(ErrorReport& report, const TypeEnvironment &typeEnv, const AstProgram& program, const AstRelation& relation);
    static void checkRules(ErrorReport& report, const TypeEnvironment &typeEnv, const AstProgram& program);

    static const AstComponent* checkComponentNameReference(ErrorReport& report, const AstComponent *enclosingComponent, const ComponentLookup &componentLookup, const std::string& name, const AstSrcLocation& loc, const TypeBinding& binding);
    static void checkComponentReference(ErrorReport& report, const AstComponent *enclosingComponent, const ComponentLookup &componentLookup, const AstComponentType& type, const AstSrcLocation& loc, const TypeBinding& binding);
    static void checkComponentInit(ErrorReport& report, const AstComponent *enclosingComponent, const ComponentLookup &componentLookup, const AstComponentInit& init, const TypeBinding& binding);
    static void checkComponent(ErrorReport& report, const AstComponent *enclosingComponent, const ComponentLookup &componentLookup, const AstComponent& component, const TypeBinding& binding);
    static void checkComponents(ErrorReport& report, const AstProgram& program, const ComponentLookup &componentLookup);

    static void checkUnionType(ErrorReport& report, const AstProgram& program, const AstUnionType& type);
    static void checkRecordType(ErrorReport& report, const AstProgram& program, const AstRecordType& type);
    static void checkType(ErrorReport& report, const AstProgram& program, const AstType& type);
    static void checkTypes(ErrorReport& report, const AstProgram& program);

public:
    virtual ~AstSemanticChecker() { }

    virtual std::string getName() const {
        return "AstSemanticChecker";
    }
};

class AstExecutionPlanChecker : public AstTransformer {
private:
    virtual bool transform(AstTranslationUnit &translationUnit);
public:
    virtual std::string getName() const {
        return "AstExecutionPlanChecker";
    }
};

