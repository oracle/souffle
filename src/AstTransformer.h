/*
 * Copyright (c) 2015, Oracle and/or its affiliates.
 *
 * All rights reserved.
 */

/************************************************************************
 *
 * @file AstTransformer.h
 *
 * Defines the interface for AST transformation passes.
 *
 ***********************************************************************/
#pragma once

#include <string>

class AstTranslationUnit;

class AstTransformer {
private:
    virtual bool transform(AstTranslationUnit &translationUnit) = 0;

public:
    virtual ~AstTransformer() { }

    bool apply(AstTranslationUnit &translationUnit);

    virtual std::string getName() const = 0;

};

