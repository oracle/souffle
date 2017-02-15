/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file AstRelation.h
 *
 * Defines class Relation that represents relations in a Datalog program.
 * A relation can either be an IDB or EDB relation.
 *
 ***********************************************************************/

#pragma once

#include "AstAttribute.h"
#include "AstClause.h"
#include "AstIODirective.h"
#include "AstNode.h"
#include "AstRelationIdentifier.h"
#include "AstType.h"

#include <iostream>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include <ctype.h>

/** Types of relation qualifiers defined as bits in a word */

/* relation is read from csv file */
#define INPUT_RELATION (0x1)

/* relation is written to csv file */
#define OUTPUT_RELATION (0x2)

/* number of tuples are written to stdout */
#define PRINTSIZE_RELATION (0x4)

/* Rules of a relation defined in a component can be overwritten by sub-component */
#define OVERRIDABLE_RELATION (0x8)

#define DATA_RELATION (0x10)

/* Relation uses a brie data structure */
#define BRIE_RELATION (0x20)

/* Relation uses a btree data structure */
#define BTREE_RELATION (0x40)

/* Relation uses a union relation */
#define EQREL_RELATION (0x80)

namespace souffle {

/*!
 * @class Relation
 * @brief Intermediate representation of a datalog relation
 *
 * A relation has a name, types of its arguments, qualifier type,
 * and dependencies to other relations.
 *
 */
class AstRelation : public AstNode {
protected:
    /** Name of relation */
    AstRelationIdentifier name;

    /** Attributes of the relation */
    std::vector<std::unique_ptr<AstAttribute>> attributes;

    /** Qualifier of relation (i.e., output or not an output relation) */
    // TODO: Change to a set of qualifiers
    int qualifier;

    /** Clauses associated with this relation. Clauses could be
      * either facts or rules.
      */
    std::vector<std::unique_ptr<AstClause>> clauses;

    /** IO directives associated with this relation.
      */
    std::vector<std::unique_ptr<AstIODirective>> ioDirectives;

public:
    AstRelation() : qualifier(0) {}

    ~AstRelation() {}

    /** Return the name of the relation */
    const AstRelationIdentifier& getName() const {
        return name;
    }

    /** Set name for this relation */
    void setName(const AstRelationIdentifier& n) {
        name = n;
    }

    /** Add a new used type to this relation */
    void addAttribute(std::unique_ptr<AstAttribute> attr) {
        ASSERT(attr && "Undefined attribute");
        attributes.push_back(std::move(attr));
    }

    /** Return the arity of this relation */
    size_t getArity() const {
        return attributes.size();
    }

    /** Return the declared type at position @p idx */
    AstAttribute* getAttribute(size_t idx) const {
        return attributes[idx].get();
    }

    /** Obtains a list of the contained attributes */
    std::vector<AstAttribute*> getAttributes() const {
        return toPtrVector(attributes);
    }

    /** Return qualifier associated with this relation */
    int getQualifier() const {
        return qualifier;
    }

    /** Set qualifier associated with this relation */
    void setQualifier(int q) {
        qualifier = q;
    }

    /** Check whether relation is an output relation */
    bool isOutput() const {
        return (qualifier & OUTPUT_RELATION) != 0;
    }

    /** Check whether relation is an input relation */
    bool isInput() const {
        return (qualifier & INPUT_RELATION) != 0;
    }

    /** Check whether relation is to/from memory */
    bool isData() const {
        return (qualifier & DATA_RELATION) != 0;
    }

    /** Check whether relation is a brie relation */
    bool isBrie() const {
        return (qualifier & BRIE_RELATION) != 0;
    }

    /** Check whether relation is a btree relation */
    bool isBTree() const {
        return (qualifier & BTREE_RELATION) != 0;
    }

    /** Check whether relation is a equivalence relation */
    bool isEqRel() const {
        return (qualifier & EQREL_RELATION) != 0;
    }

    /** Check whether relation is an input relation */
    bool isPrintSize() const {
        return (qualifier & PRINTSIZE_RELATION) != 0;
    }

    /** Check whether relation is an output relation */
    bool isComputed() const {
        return isOutput() || isPrintSize();
    }

    /** Check whether relation is an overridable relation */
    bool isOverridable() const {
        return (qualifier & OVERRIDABLE_RELATION) != 0;
    }

    /** Print string representation of the relation to a given output stream */
    virtual void print(std::ostream& os) const {
        os << ".decl " << this->getName() << "(";
        if (attributes.size() > 0) {
            os << attributes[0]->getAttributeName() << ":" << attributes[0]->getTypeName();

            for (size_t i = 1; i < attributes.size(); ++i) {
                os << "," << attributes[i]->getAttributeName() << ":" << attributes[i]->getTypeName();
            }
        }
        os << ") ";
        if (isInput()) {
            os << "input ";
        }
        if (isOutput()) {
            os << "output ";
        }
        if (isData()) {
            os << "output ";
        }
        if (isPrintSize()) {
            os << "printsize ";
        }
        if (isOverridable()) {
            os << "overridable ";
        }
    }

    /** Creates a clone if this AST sub-structure */
    virtual AstRelation* clone() const {
        auto res = new AstRelation();
        res->name = name;
        res->setSrcLoc(getSrcLoc());
        for (const auto& cur : attributes)
            res->attributes.push_back(std::unique_ptr<AstAttribute>(cur->clone()));
        for (const auto& cur : clauses) res->clauses.push_back(std::unique_ptr<AstClause>(cur->clone()));
        for (const auto& cur : ioDirectives) {
            res->ioDirectives.push_back(std::unique_ptr<AstIODirective>(cur->clone()));
        }
        res->qualifier = qualifier;
        return res;
    }

    /** Mutates this node */
    virtual void apply(const AstNodeMapper& map) {
        for (auto& cur : attributes) {
            cur = map(std::move(cur));
        }
        for (auto& cur : clauses) {
            cur = map(std::move(cur));
        }
        for (auto& cur : ioDirectives) {
            cur = map(std::move(cur));
        }
    }

    /** Return i-th clause associated with this relation */
    AstClause* getClause(size_t idx) const {
        return clauses[idx].get();
    }

    /** Obtains a list of the associated clauses */
    std::vector<AstClause*> getClauses() const {
        return toPtrVector(clauses);
    }

    /** Add a clause to the relation */
    void addClause(std::unique_ptr<AstClause> clause) {
        ASSERT(clause && "Undefined clause");
        ASSERT(clause->getHead() && "Undefined head of the clause");
        ASSERT(clause->getHead()->getName() == name &&
                "Name of the atom in the head of the clause and the relation do not match");
        clauses.push_back(std::move(clause));
    }

    /** Removes the given clause from this relation */
    bool removeClause(const AstClause* clause) {
        for (auto it = clauses.begin(); it != clauses.end(); ++it) {
            if (**it == *clause) {
                clauses.erase(it);
                return true;
            }
        }
        return false;
    }

    /** Return the number of clauses associated with this relation */
    size_t clauseSize() const {
        return clauses.size();
    }

    /** Obtains a list of all embedded child nodes */
    virtual std::vector<const AstNode*> getChildNodes() const {
        std::vector<const AstNode*> res;
        for (const auto& cur : attributes) {
            res.push_back(cur.get());
        }
        for (const auto& cur : clauses) {
            res.push_back(cur.get());
        }
        for (const auto& cur : ioDirectives) {
            res.push_back(cur.get());
        }
        return res;
    }

    void addIODirectives(std::unique_ptr<AstIODirective> directive) {
        ASSERT(directive && "Undefined directive");
        // Make sure the old style qualifiers still work.
        if (directive->isInput()) {
            qualifier |= INPUT_RELATION;
        } else if (directive->isOutput()) {
            qualifier |= OUTPUT_RELATION;
        } else if (directive->isPrintSize()) {
            qualifier |= PRINTSIZE_RELATION;
        }
        ioDirectives.push_back(std::move(directive));
    }

    std::vector<AstIODirective*> getIODirectives() const {
        return toPtrVector(ioDirectives);
    }

protected:
    /** Implements the node comparison for this node type */
    virtual bool equal(const AstNode& node) const {
        assert(dynamic_cast<const AstRelation*>(&node));
        const AstRelation& other = static_cast<const AstRelation&>(node);
        return name == other.name && equal_targets(attributes, other.attributes) &&
               equal_targets(clauses, other.clauses);
    }
};

struct AstNameComparison {
    bool operator()(const AstRelation* x, const AstRelation* y) const {
        if (x != nullptr && y != nullptr) {
            return x->getName() < y->getName();
        } else {
            return y != nullptr;
        }
    }
};

typedef std::set<const AstRelation*, AstNameComparison> AstRelationSet;

}  // end of namespace souffle
