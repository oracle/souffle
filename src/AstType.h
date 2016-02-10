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
 * @file AstType.h
 *
 * Defines a type, i.e., disjoint supersets of the universe
 *
 ***********************************************************************/

#pragma once

#include <ctype.h>

#include <iostream>
#include <string>
#include <vector>
#include <set>

#include "AstNode.h"

/** The kind of type utilized as an identifier for AST types */
typedef std::string AstTypeIdentifier;

/**
 *  @class Type
 *  @brief An abstract base class for types within the AST.
 *
 */
class AstType : public AstNode {

    /** In the AST each type has to have a name forming a unique identifier */
    AstTypeIdentifier name;

public:

    /** Creates a new type */
    AstType(const AstTypeIdentifier& name = "") : name(name) {}


    /** Obtains the name of this type */
    const AstTypeIdentifier& getName() const {
        return name;
    }

    /** Updates the name of this type */
    void setName(const AstTypeIdentifier& name) {
        this->name = name;
    }

    /** Obtains a list of all embedded child nodes */
    virtual std::vector<const AstNode*> getChildNodes() const {
        return {};
    }

    /** Creates a clone if this AST sub-structure */
    virtual AstType* clone() const =0;

    /** Mutates this node */
    virtual void apply(const AstNodeMapper& map) {
        // no nested nodes in any type
    }

};

/**
 * A primitive type is named type that can either be a sub-type of
 * the build-in number or symbol type. Primitive types are the most
 * basic building blocks of souffle's type system.
 */
class AstPrimitiveType : public AstType {

    /** Indicates whether it is a number (true) or a symbol (false) */
    bool num;

public:

    /** Creates a new primitive type */
    AstPrimitiveType(const AstTypeIdentifier& name, bool num = false)
        : AstType(name), num(num) {}

    /** Tests whether this type is a numeric type */
    bool isNumeric() const {
        return num;
    }

    /** Tests whether this type is a symbolic type */
    bool isSymbolic() const {
        return !num;
    }

    /** Prints a summary of this type to the given stream */
    virtual void print(std::ostream &os) const {
        os << ".type " << getName() << (num ? "= number" : "");
    }

    /** Creates a clone if this AST sub-structure */
    virtual AstPrimitiveType* clone() const {
        return new AstPrimitiveType(getName(), num);
    }

protected:

    /** Implements the node comparison for this node type */
    virtual bool equal(const AstNode& node) const {
        assert(dynamic_cast<const AstPrimitiveType*>(&node));
        const AstPrimitiveType& other = static_cast<const AstPrimitiveType&>(node);
        return getName() == other.getName() && num == other.num;
    }

};

/**
 * A union type combines multiple types into a new super type.
 * Each of the enumerated types become a sub-type of the new
 * union type.
 */
class AstUnionType : public AstType {

    /** The list of types aggregated by this union type */
    std::vector<AstTypeIdentifier> types;

public:

    /** Creates a new union type */
    AstUnionType() {}

    /** Obtains a reference to the list element types */
    const std::vector<AstTypeIdentifier>& getTypes() const {
        return types;
    }

    /** Adds another element type */
    void add(const AstTypeIdentifier& type) {
        types.push_back(type);
    }

    /** Prints a summary of this type to the given stream */
    virtual void print(std::ostream& os) const {
        os << ".type " << getName() << " = " << join(types, " | ");
    }

    /** Creates a clone if this AST sub-structure */
    virtual AstUnionType* clone() const {
        auto res = new AstUnionType();
        res->setName(getName());
        res->types = types;
        return res;
    }

protected:

    /** Implements the node comparison for this node type */
    virtual bool equal(const AstNode& node) const {
        assert(dynamic_cast<const AstUnionType*>(&node));
        const AstUnionType& other = static_cast<const AstUnionType&>(node);
        return getName() == other.getName() && types == other.types;
    }

};

/**
 * A record type aggregates a list of fields into a new type.
 * Each record type has a name making it unique. Two record
 * types are unrelated to all other types (they do not have
 * any super or sub types).
 */
struct AstRecordType : public AstType {

    /** The type utilized to model a field */
    struct Field {
        std::string name;           // < the field name
        AstTypeIdentifier type;     // < the field type

        bool operator==(const Field& other) const {
            return this == &other || (name == other.name && type == other.type);
        }
    };

private:

    /** The list of fields constituting this record type */
    std::vector<Field> fields;

public:

    /** Creates a new record type */
    AstRecordType() {}

    /** Adds a new field to this record type */
    void add(const std::string& name, const AstTypeIdentifier& type) {
        fields.push_back(Field({name, type}));
    }

    /** Obtains the list of field constituting this record type */
    const std::vector<Field>& getFields() const {
        return fields;
    }

    /** Prints a summary of this type to the given stream */
    virtual void print(std::ostream& os) const {
        os << ".type " << getName() << " = " << "[";
        for(unsigned i=0; i<fields.size(); i++) {
            if (i!=0) os << ",";
            os << fields[i].name;
            os << ":";
            os << fields[i].type;
        }
        os << "]";
    }

    /** Creates a clone if this AST sub-structure */
    virtual AstRecordType* clone() const {
        auto res = new AstRecordType();
        res->setName(getName());
        res->fields = fields;
        return res;
    }

protected:

    /** Implements the node comparison for this node type */
    virtual bool equal(const AstNode& node) const {
        assert(dynamic_cast<const AstRecordType*>(&node));
        const AstRecordType& other = static_cast<const AstRecordType&>(node);
        return getName() == other.getName() && fields == other.fields;
    }

};

