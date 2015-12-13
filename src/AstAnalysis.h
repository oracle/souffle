/*
 * Copyright (c) 2015, Oracle and/or its affiliates.
 *
 * All rights reserved.
 */

/************************************************************************
 *
 * @file AstAnalysis.h
 *
 * Defines the interface for AST analysis passes.
 *
 ***********************************************************************/
#pragma once

class AstTranslationUnit;

class AstAnalysis {
public:


    virtual ~AstAnalysis() { }

    virtual void run(const AstTranslationUnit &translationUnit) = 0;
};
