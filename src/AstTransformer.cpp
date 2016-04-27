/*
 * Souffle version 0.0.0
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - souffle/LICENSE
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

namespace souffle {

bool AstTransformer::apply(AstTranslationUnit& translationUnit)  {
    bool changed = transform(translationUnit);
    if (changed) {
        translationUnit.invalidateAnalyses();
    }
    return changed;
}

} // end of namespace souffle

