/*
 * Souffle version 0.0.0
 * Copyright (c) 2013, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - souffle/LICENSE
 */

/************************************************************************
 *
 * @file ComponentInstantiation.h
 *
 ***********************************************************************/

#pragma once

#include <string>
#include <vector>
#include <memory>

#include "AstRelation.h"
#include "AstAnalysis.h"
#include "AstTranslationUnit.h"
#include "AstComponent.h"

namespace souffle {

class ErrorReport;

/**
 * Class that encapsulates std::map of types binding that comes from .init c = Comp<MyType>
 * Type binding in this example would be T->MyType if the component code is .comp Comp<T> ...
 */
class TypeBinding {

    /**
     * Key value pair. Keys are names that should be forwarded to value,
     * which is the actual name. Example T->MyImplementation.
     */
    std::map<std::string,std::string> binding;

public:

    /**
     * Returns binding for given name or empty string if such binding does not exist.
     */
    const std::string find(const std::string& name) const {
        auto pos = binding.find(name);
        if (pos == binding.end()) return std::string();
        return pos->second;
    }

    TypeBinding extend(const std::vector<std::string>& formalParams, const std::vector<std::string>& actualParams) const {
        TypeBinding result;
        if (formalParams.size() != actualParams.size())
            return *this;     // invalid init => will trigger a semantic error

        for(std::size_t i=0; i<formalParams.size(); i++) {
            auto pos = binding.find(actualParams[i]);
            if (pos != binding.end()) {
                result.binding[formalParams[i]] = pos->second;
            } else {
                result.binding[formalParams[i]] = actualParams[i];
            }
        }

        return result;
    }
};

class ComponentLookup : public AstAnalysis {
private:
    std::set<const AstComponent *> globalScopeComponents; // components defined outside of any components
    std::map<const AstComponent *, std::set<const AstComponent *>> nestedComponents; // components defined inside a component
    std::map<const AstComponent *, const AstComponent *> enclosingComponent; // component definition enclosing a component definition
public:
    static constexpr const char *name = "component-lookup";

    virtual void run(const AstTranslationUnit &translationUnit);

    /**
     * Performs a lookup operation for a component with the given name within the addressed scope.
     *
     * @param scope the component scope to lookup in (null for global scope)
     * @param name the name of the component to be looking for
     * @return a pointer to the obtained component or null if there is no such component.
     */
    const AstComponent* getComponent(const AstComponent *scope, const std::string& name, const TypeBinding& activeBinding) const;
};

class ComponentInstantiationTransformer : public AstTransformer {
private:
    virtual bool transform(AstTranslationUnit &translationUnit);

    static const unsigned int MAX_INSTANTIATION_DEPTH = 1000;

    /**
     * Recursively computes the set of relations (and included clauses) introduced
     * by this init statement enclosed within the given scope.
     */
    static std::vector<std::unique_ptr<AstRelation>> getInstantiatedRelations(const AstComponentInit &componentInit, const AstComponent *enclosingComponent, const ComponentLookup &componentLookup, std::vector<std::unique_ptr<AstClause>> &orphans, ErrorReport &report, const TypeBinding& binding = TypeBinding(), unsigned int maxDepth = MAX_INSTANTIATION_DEPTH);

    /**
     * Collects clones of all relations in the given component and its base components.
     */
    static void collectAllRelations(const AstComponent& component, const TypeBinding& binding, const AstComponent *enclosingComponent, const ComponentLookup &componentLookup, std::vector<std::unique_ptr<AstRelation>>& res, std::vector<std::unique_ptr<AstClause>> &orphans, std::set<std::string> overridden, ErrorReport &report, unsigned int maxInstantiationDepth);
public:
    std::string getName() const {
        return "ComponentInstantiationTransformer";
    }
};

} // end of namespace souffle

