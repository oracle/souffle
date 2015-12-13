/*
 * Copyright (c) 2015, Oracle and/or its affiliates.
 *
 * All rights reserved.
 */

/************************************************************************
 *
 * @file AstTransformer.cpp
 *
 * Defines the interface for AST transformation passes.
 *
 ***********************************************************************/
#include "AstTransformer.h"
#include "AstTranslationUnit.h"

bool AstTransformer::apply(AstTranslationUnit& translationUnit)  {
    bool changed = transform(translationUnit);
    if (changed) {
        translationUnit.invalidateAnalyses();
    }
    return changed;
}
