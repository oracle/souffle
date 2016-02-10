/*
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All Rights reserved
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
 * @file TypeSystem.h
 *
 * Covers basic operations constituting Souffle's type system.
 *
 ***********************************************************************/

#pragma once

#include <iostream>
#include <string>
#include <set>
#include <map>
#include <vector>
#include <initializer_list>

#include "Util.h"
#include "IterUtils.h"

// forward declaration
struct TypeEnvironment;

/**
 * An abstract base class for types to be covered within a type environment.
 */
class Type {

protected:

    /** A reference to the type environment this type is associated to. */
    const TypeEnvironment& environment;

private:

    /** The name of this type. */
    std::string name;

public:

    Type(const TypeEnvironment& environment, const std::string& name)
        : environment(environment), name(name) {}

    Type(const Type& other) = delete;

    virtual ~Type() {}

    const std::string& getName() const {
        return name;
    }

    const TypeEnvironment& getTypeEnvironment() const {
        return environment;
    }

    bool operator==(const Type& other) const {
        return this == &other;
    }

    bool operator!=(const Type& other) const {
        return !(*this == other);
    }

    bool operator<(const Type& other) const {
        return name < other.name;
    }

    virtual void print(std::ostream& out = std::cout) const {
        out << name;
    }

    friend std::ostream& operator<<(std::ostream& out, const Type& t) {
        return t.print(out), out;
    }

    friend std::ostream& operator<<(std::ostream& out, const Type* t) {
        if (!t) return out << "-null-";
        return t->print(out), out;
    }
};

/**
 * A primitive type. The basic type construct to build new types.
 */
class PrimitiveType : public Type {

    // only allow type environments to create instances
    friend struct TypeEnvironment;

    /** The base type -- may be symbol or numerical */
    const Type& baseType;

    PrimitiveType(const TypeEnvironment& environment, const std::string& name, const Type& base)
        : Type(environment, name), baseType(base) {}

public:

    void print(std::ostream& out) const;

    const Type& getBaseType() const {
        return baseType;
    }
};

/**
 * A union type combining a list of types into a new, aggregated type.
 */
class UnionType : public Type {

    // only allow type environments to create instances
    friend struct TypeEnvironment;

    /** The contained element types */
    std::vector<const Type*> elementTypes;

    UnionType(const TypeEnvironment& environment, const std::string& name) : Type(environment, name) {}

public:

    void add(const Type& type);

    const std::vector<const Type*>& getElementTypes() const {
        return elementTypes;
    }

    void print(std::ostream& out) const;
};

/**
 * A record type combining a list of fields into a new, aggregated type.
 */
struct RecordType : public Type {

    // only allow type environments to create instances
    friend struct TypeEnvironment;

    /** The type to model fields */
    struct Field {
        std::string name;   // < the name of the field
        const Type& type;   // < the type of the field
    };

private:

    /** The list of contained fields */
    std::vector<Field> fields;

    RecordType(const TypeEnvironment& environment, const std::string& name) : Type(environment, name) {}

public:

    void add(const std::string& name, const Type& type);

    const std::vector<Field>& getFields() const { return fields; }

    void print(std::ostream& out) const;
};


/**
 * A collection to represent sets of types. In addition to ordinary set capabilities
 * it may also represent the set of all types -- without being capable of iterating over those.
 *
 * It is the basic entity to conduct sub- and super-type computations.
 */
struct TypeSet {

    typedef IterDerefWrapper<typename std::set<const Type*>::const_iterator> const_iterator;

private:

    /** True if it is the all-types set, false otherwise */
    bool all;

    /** The enumeration of types in case it is not the all-types set */
    std::set<const Type*, deref_less<Type>> types;

public:

    // -- constructors, destructors and assignment operations --

    TypeSet(bool all = false) : all(all) {}

    TypeSet(const TypeSet& other) = default;

    TypeSet(TypeSet&& other) : all(other.all), types() {
        types.swap(other.types);
    }

    template<typename ... Types>
    TypeSet(const Types& ... types) : all(false) {
        for(const Type* cur : toVector<const Type*>(&types... )) {
            this->types.insert(cur);
        }
    }

    TypeSet& operator=(const TypeSet& other) =default;


    /** A factory function for the all-types set */
    static TypeSet getAllTypes() {
        return TypeSet(true);
    }

    /** Emptiness check */
    bool empty() const {
        return !all && types.empty();
    }

    /** Universality check */
    bool isAll() const {
        return all;
    }

    /** Determines the size of this set unless it is the universal set */
    std::size_t size() const {
        assert(!all && "Unable to give size of universe.");
        return types.size();
    }

    /** Determines whether a given type is included or not */
    bool contains(const Type& type) const {
        return all || types.find(&type) != types.end();
    }

    /** Adds the given type to this set */
    void insert(const Type& type) {
        if (all) return;
        types.insert(&type);
    }

    /** Inserts all the types of the given set into this set */
    void insert(const TypeSet& set) {
        if (all) return;

        // if the other set is universal => make this one universal
        if (set.isAll()) {
            all = true;
            types.clear();
            return;
        }

        // add types one by one
        for(const auto& t : set) insert(t);
    }

    /** Allows to iterate over the types contained in this set (only if not universal) */
    const_iterator begin() const {
        assert(!all && "Unable to enumerate universe.");
        return derefIter(types.begin());
    }

    /** Allows to iterate over the types contained in this set (only if not universal) */
    const_iterator end() const {
        assert(!all && "Unable to enumerate universe.");
        return derefIter(types.end());
    }

    /** Determines whether this set is a subset of the given set */
    bool isSubsetOf(const TypeSet& b) const {
        if (all) return b.isAll();
        return all_of(*this, [&](const Type& cur) { return b.contains(cur); });
    }

    /** Determines equality between type sets */
    bool operator==(const TypeSet& other) const {
        return all == other.all && types == other.types;
    }

    /** Determines inequality between type sets */
    bool operator!=(const TypeSet& other) const {
        return !(*this == other);
    }

    /** Adds print support for type sets */
    void print(std::ostream& out) const {
        if (all) {
            out << "{ - all types - }";
        } else {

            out << "{" << join(types,",",[](std::ostream& out, const Type* type) {
                out << type->getName();
            }) << "}";
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const TypeSet& set) {
        set.print(out);
        return out;
    }
};


/**
 * A type environment is a set of types. It's main purpose is to provide an enumeration
 * of all all types within a given program. Additionally, it manages the life cycle of
 * type instances.
 */
class TypeEnvironment {

    /** The type utilized for identifying types */
    typedef std::string identifier;

private:

    /** The list of covered types */
    std::map<identifier,Type*> types;

public:

    // -- constructors / destructores --
    TypeEnvironment();

    TypeEnvironment(const TypeEnvironment&) =delete;

    ~TypeEnvironment();


    // -- create types in this environment --

    template<typename T, typename ... Args>
    T& createType(const identifier& name, const Args& ... args) {
        T* res = new T(*this, name, args...);
        addType(*res);
        return *res;
    }

    PrimitiveType& createNumericType(const identifier& name) {
        return createType<PrimitiveType>(name, getNumberType());
    }

    PrimitiveType& createSymbolType(const identifier& name) {
        return createType<PrimitiveType>(name, getSymbolType());
    }

    UnionType& createUnionType(const identifier& name) {
        return createType<UnionType>(name);
    }

    RecordType& createRecordType(const identifier& name) {
        return createType<RecordType>(name);
    }

    // -- query type information --

    bool isType(const identifier& name) const;

    bool isType(const Type& type) const;

    const Type& getType(const identifier& name) const;

    const Type& getNumberType() const { return getType("number"); }

    const Type& getSymbolType() const { return getType("symbol"); }

    TypeSet getAllTypes() const;

    Type* getModifiableType(const identifier& name);

    void clear();

    void print(std::ostream& out) const;

    friend std::ostream& operator<<(std::ostream& out, const TypeEnvironment& environment) {
        environment.print(out);
        return out;
    }

    void swap(TypeEnvironment& env) {
        types.swap(env.types);
    }

private:

    /** Register types created by one of the factory functions */
    void addType(Type& type);

};



// ---------------------------------------------------------------
//                          Type Utilities
// ---------------------------------------------------------------

/**
 * Returns full type qualifier for a given type
 */
const std::string getTypeQualifier(const Type& type);

/**
 * Determines whether the given type is a number type.
 */
bool isNumberType(const Type& type);

/**
 * Determines whether all the types in the given set are number types.
 */
bool isNumberType(const TypeSet& s);

/**
 * Determines whether the given type is a symbol type.
 */
bool isSymbolType(const Type& type);

/**
 * Determines whether all the types in the given set are symbol types.
 */
bool isSymbolType(const TypeSet& s);

/**
 * Determines whether the given type is a record type.
 */
bool isRecordType(const Type& type);

/**
 * Determines whether all the types in the given set are record types.
 */
bool isRecordType(const TypeSet& s);

/**
 * Determines whether the given type is a recursive type.
 */
bool isRecursiveType(const Type& type);

/**
 * Determines whether type a is a subtype of type b.
 */
bool isSubtypeOf(const Type& a, const Type& b);

/**
 * Determines whether all types in s are subtypes of type b.
 */
bool areSubtypesOf(const TypeSet& s, const Type& b);



// -- Least Common Super Types ----------------------------------------

/**
 * Computes the least common super types of the two given types.
 */
TypeSet getLeastCommonSupertypes(const Type& a, const Type& b);

/**
 * Computes the least common super types of all the types in the given set.
 */
TypeSet getLeastCommonSupertypes(const TypeSet& set);

/**
 * The set of pair-wise least common super types of the types in the two given sets.
 */
TypeSet getLeastCommonSupertypes(const TypeSet& a, const TypeSet& b);

/**
 * Computes the least common super types of the given types.
 */
template<typename ... Types>
TypeSet getLeastCommonSupertypes(const Types& ... types) {
    return getLeastCommonSupertypes(TypeSet(types...));
}

// -- Greatest Common Sub Types --------------------------------------

/**
 * Computes the greatest common sub types of the two given types.
 */
TypeSet getGreatestCommonSubtypes(const Type& a, const Type& b);

/**
 * Computes the greatest common sub types of all the types in the given set.
 */
TypeSet getGreatestCommonSubtypes(const TypeSet& set);

/**
 * The set of pair-wise greatest common sub types of the types in the two given sets.
 */
TypeSet getGreatestCommonSubtypes(const TypeSet& a, const TypeSet& b);

/**
 * Computes the greatest common sub types of the given types.
 */
template<typename ... Types>
TypeSet getGreatestCommonSubtypes(const Types& ... types) {
    return getGreatestCommonSubtypes(TypeSet(types...));
}
