/*
 * Copyright (c) 2013-14, Oracle and/or its affiliates.
 *
 * All rights reserved.
 */

/************************************************************************
 *
 * @file AstAttribute.h
 *
 * Defines an attribute for a relation
 *
 ***********************************************************************/

#pragma once

#include <ctype.h>

#include <iostream>
#include <string>
#include <vector>
#include <set>

#include "AstNode.h"
#include "AstType.h"

// forward declaration
class Type;

/**
 *  @class Attribute
 *  @brief Intermediate representation of an attribute which stores the name and the type of an attribute
 *
 *  Attribute has the only name attribute
 */
class AstAttribute : public AstNode {

    /** Attribute name */
    std::string name;

    /** Type name */ 
    std::string typeName; 

public:

    AstAttribute(const std::string& n, const std::string& t, const Type* type = NULL)
        : name(n), typeName(t) {}

    const std::string& getAttributeName() const {
        return name; 
    }

    const std::string& getTypeName() const {
        return typeName; 
    }

    void setTypeName(const std::string& name) {
        typeName = name;
    }

    virtual void print(std::ostream &os) const {
        os << name << ":" << typeName;
    }

    /** Creates a clone if this AST sub-structure */
    virtual AstAttribute* clone() const {
        AstAttribute* res = new AstAttribute(name, typeName);
        res->setSrcLoc(getSrcLoc());
        return res;
    }

    /** Mutates this node */
    virtual void apply(const AstNodeMapper& map) {
        // no nested AST nodes
    }

    /** Obtains a list of all embedded child nodes */
    virtual std::vector<const AstNode*> getChildNodes() const {
        return std::vector<const AstNode*>();
    }

protected:

    /** Implements the node comparison for this node type */
    virtual bool equal(const AstNode& node) const {
        assert(dynamic_cast<const AstAttribute*>(&node));
        const AstAttribute& other = static_cast<const AstAttribute&>(node);
        return name == other.name && typeName == other.typeName;
    }

};
