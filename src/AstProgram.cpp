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
 * @file AstProgram.h
 *
 * Implement methods for class Program that represents a Datalog
 * program consisting of types, relations, and clauses.
 *
 ***********************************************************************/

#include <stdlib.h>
#include <stdarg.h>
#include <sstream>

#include <list>

#include "Util.h"
#include "GraphUtils.h"

#include "AstProgram.h"
#include "AstRelation.h"
#include "AstClause.h"
#include "AstComponent.h"
#include "AstVisitor.h"
#include "AstUtils.h"
#include "AstTypeAnalysis.h"
#include "ErrorReport.h"

/*
 * Program
 */

AstProgram::AstProgram(AstProgram&& other) {
    types.swap(other.types);
    relations.swap(other.relations);
    clauses.swap(other.clauses);
    components.swap(other.components);
    instantiations.swap(other.instantiations);
}

/* Add a new type to the program */
void AstProgram::addType(std::unique_ptr<AstType> type)
{
    auto& cur = types[type->getName()];
    assert(!cur && "Redefinition of type!");
    cur = std::move(type);
}

const AstType* AstProgram::getType(const std::string& name) const {
    auto pos = types.find(name);
    return (pos == types.end()) ? nullptr : pos->second.get();
}

std::vector<const AstType*> AstProgram::getTypes() const {
    std::vector<const AstType*> res;
    for(const auto& cur : types) res.push_back(cur.second.get());
    return res;
}

/* Add a relation to the program */
void AstProgram::addRelation(std::unique_ptr<AstRelation> r)
{
    const auto& name = r->getName();
    assert(relations.find(name) == relations.end() && "Redefinition of relation!");
    relations[name] = std::move(r);
}

void AstProgram::appendRelation(std::unique_ptr<AstRelation> r) {

    // get relation
    std::unique_ptr<AstRelation>& rel = relations[r->getName()];
    assert(!rel && "Adding pre-existing relation!");

    // add relation
    rel = std::move(r);

}

/* Remove a relation from the program */
void AstProgram::removeRelation(const AstRelationIdentifier &name)
{
    /* Remove relation from map */
    relations.erase(relations.find(name));
}

void AstProgram::appendClause(std::unique_ptr<AstClause> clause) {

    // get relation
    std::unique_ptr<AstRelation> &r = relations[clause->getHead()->getName()];
    assert(r && "Trying to append to unknown relation!");

    // delegate call
    r->addClause(std::move(clause));
}

void AstProgram::removeClause(const AstClause* clause) {

    // get relation
    auto pos = relations.find(clause->getHead()->getName());
    if (pos == relations.end()) return;

    // delegate call
    pos->second->removeClause(clause);

}

AstRelation* AstProgram::getRelation(const AstRelationIdentifier& name) const {
    auto pos = relations.find(name);
    return (pos == relations.end()) ? nullptr : pos->second.get();
}

/* Add a clause to the program */
void AstProgram::addClause(std::unique_ptr<AstClause> clause) {
    ASSERT(clause && "NULL clause");
    clauses.push_back(std::move(clause));
}

/* Put all relations of the program into a list */
std::vector<AstRelation*> AstProgram::getRelations() const
{
    std::vector<AstRelation*> res;
    for(const auto& rel : relations) {
        res.push_back(rel.second.get());
    }
    return res;
}

/* Print program in textual format */
void AstProgram::print(std::ostream &os) const
{
    /* Print types */
    os << "// ----- Types -----\n";
    for(const auto& cur : types) {
    	os << *cur.second << "\n";
    }

    /* Print components */
    if (!components.empty()) {
        os << "\n// ----- Components -----\n";
        for(const auto& cur : components) {
            os << *cur << "\n";
        }
    }

    /* Print instantiations */
    if (!instantiations.empty()) {
        os << "\n";
        for(const auto& cur : instantiations) {
            os << *cur << "\n";
        }
    }

    /* Print relations */
    os << "\n// ----- Relations -----\n";
    for(const auto& cur : relations) {
        const std::unique_ptr<AstRelation> &rel = cur.second;
        os << "\n\n// -- " << rel->getName() << " --\n" ;
        os << *rel << "\n\n";
        for (size_t i = 0 ; i< rel->clauseSize(); i++) {
            os << *rel->getClause(i) << "\n\n";
        }
    }

    if (!clauses.empty()) {
        os << "\n// ----- Orphan Clauses -----\n";
        os << join(clauses, "\n\n", print_deref<std::unique_ptr<AstClause>>()) << "\n";
    }
}

AstProgram* AstProgram::clone() const {

    // create copy
    auto res = new AstProgram();

    // move types
    for(const auto& cur : types) {
        res->types.insert(std::make_pair(cur.first, std::unique_ptr<AstType>(cur.second->clone())));
    }

    // move relations
    for(const auto& cur : relations) {
        res->relations.insert(std::make_pair(cur.first, std::unique_ptr<AstRelation>(cur.second->clone())));
    }

    // move components
    for(const auto& cur : components) {
        res->components.push_back(std::unique_ptr<AstComponent>(cur->clone()));
    }

    // move component instantiations
    for(const auto& cur : instantiations) {
        res->instantiations.push_back(std::unique_ptr<AstComponentInit>(cur->clone()));
    }

    ErrorReport errors;

    res->finishParsing();

    // done
    return res;

}

/** Mutates this node */
void AstProgram::apply(const AstNodeMapper& map) {
    for(auto& cur : types) {
        cur.second = map(std::move(cur.second));
    }
    for(auto& cur : relations) {
        cur.second = map(std::move(cur.second));
    }
    for(auto& cur : components) {
        cur = map(std::move(cur));
    }
    for(auto& cur : instantiations) {
        cur = map(std::move(cur));
    }
}

void AstProgram::finishParsing() {
    // unbound clauses with no relation defined
    std::vector<std::unique_ptr<AstClause>> unbound;

    // add clauses
    for(auto& cur : clauses) {
        auto pos = relations.find(cur->getHead()->getName());
        if (pos != relations.end()) {
            pos->second->addClause(std::move(cur));
        } else {
            unbound.push_back(std::move(cur));
        }
    }
    // remember the remaining orphan clauses
    clauses.clear();
    clauses.swap(unbound);
}

