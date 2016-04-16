/*
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All Rights reserved
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

namespace souffle {

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

} // end of namespace souffle

