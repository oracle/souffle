/*
 * Copyright (c) 2013, Oracle and/or its affiliates. All Rights reserved
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
 * Define a class that represents a Datalog program consisting of types,
 * relations, and clauses.
 *
 ***********************************************************************/

#pragma once

#include <map>
#include <string>
#include <list>
#include <set>
#include <memory>

#include "Util.h"
#include "AstRelation.h"
#include "AstComponent.h"
#include "TypeSystem.h"
#include "ErrorReport.h"

namespace souffle {

class AstClause;
class AstRelation;
class AstLiteral;
class AstAtom;
class AstArgument;

/**
 *  @class Program
 *  @brief Intermediate representation of a datalog program
 *          that consists of relations, clauses and types
 */
class AstProgram : public AstNode {
      using SymbolTable = souffle::SymbolTable; // XXX pending namespace cleanup

      // TODO: Check whether this is needed
      friend class ParserDriver;
      friend class ComponentInstantiationTransformer;

      /** Program types  */
      std::map<std::string, std::unique_ptr<AstType>> types;

      /** Program relations */
      std::map<AstRelationIdentifier, std::unique_ptr<AstRelation>> relations;

      /** The list of clauses provided by the user */
      std::vector<std::unique_ptr<AstClause>> clauses;

      /** Program components */
      std::vector<std::unique_ptr<AstComponent>> components;

      /** Component instantiations */
      std::vector<std::unique_ptr<AstComponentInit>> instantiations;

      /** a private constructor to restrict creation */
      AstProgram() { }

   public:

      /** Deleted copy constructor since instances can not be copied */
      AstProgram(const AstProgram&) = delete;

      /** A move constructor */
      AstProgram(AstProgram&&);

      /** A programs destructor */
      ~AstProgram() { }

      // -- Types ----------------------------------------------------------------

   private:

      /** Add the given type to the program. Asserts if a type with the
        same name has already been added.  */
      void addType(std::unique_ptr<AstType> type);

   public:

      /** Obtains the type with the given name */
      const AstType* getType(const std::string& name) const;

      /** Gets a list of all types in this program */
      std::vector<const AstType*> getTypes() const;


      // -- Relations ------------------------------------------------------------

   private:

      /** Add the given relation to the program. Asserts if a relation with the
       * same name has already been added. */
      void addRelation(std::unique_ptr<AstRelation> r);

      /** Add a clause to the program */
      void addClause(std::unique_ptr<AstClause> r);

   public:

      /** Find and return the relation in the program given its name */
      AstRelation* getRelation(const AstRelationIdentifier& name) const;

      /** Get all relations in the program */
      std::vector<AstRelation *> getRelations() const;

      /** Return the number of relations in the program */
      size_t relationSize() const { return relations.size(); }

      /** appends a new relation to this program -- after parsing */
      void appendRelation(std::unique_ptr<AstRelation> r);

      /** Remove a relation from the program. */
      void removeRelation(const AstRelationIdentifier &r);

      /** append a new clause to this program -- after parsing */
      void appendClause(std::unique_ptr<AstClause> clause);

      /** Removes a clause from this program */
      void removeClause(const AstClause* clause);

      /**
       * Obtains a list of clauses not associated to any relations. In
       * a valid program this list is always empty
       */
      std::vector<AstClause *> getOrphanClauses() const {
          return toPtrVector(clauses);
      }

      // -- Components ------------------------------------------------------------

   private:

      /** Adds the given component to this program */
      void addComponent(std::unique_ptr<AstComponent> c) { components.push_back(std::move(c)); }

      /** Adds a component instantiation */
      void addInstantiation(std::unique_ptr<AstComponentInit> i) { instantiations.push_back(std::move(i)); }

   public:

      /** Obtains a list of all comprised components */
      std::vector<AstComponent *> getComponents() const { return toPtrVector(components); }

      /** Obtains a list of all component instantiations */
      std::vector<AstComponentInit *> getComponentInstantiations() const { return toPtrVector(instantiations); }

      // -- I/O ------------------------------------------------------------------

      /** Output the program to a given output stream */
      void print(std::ostream &os) const;


      // -- Manipulation ---------------------------------------------------------

      /** Creates a clone if this AST sub-structure */
      virtual AstProgram* clone() const;

      /** Mutates this node */
      virtual void apply(const AstNodeMapper& map);

   public:

      /** Obtains a list of all embedded child nodes */
      virtual std::vector<const AstNode*> getChildNodes() const {
          std::vector<const AstNode*> res;
          for(const auto& cur : types) res.push_back(cur.second.get());
          for(const auto& cur : relations) res.push_back(cur.second.get());
          for(const auto& cur : components) res.push_back(cur.get());
          for(const auto& cur : instantiations) res.push_back(cur.get());
          for(const auto& cur : clauses) res.push_back(cur.get());
          return res;
      }

   private:

      void finishParsing();


   protected:

       /** Implements the node comparison for this node type */
       virtual bool equal(const AstNode& node) const {
           assert(dynamic_cast<const AstProgram*>(&node));
           const AstProgram& other = static_cast<const AstProgram&>(node);

           // check list sizes
           if (types.size() != other.types.size()) return false;
           if (relations.size() != other.relations.size()) return false;

           // check types
           for(const auto& cur : types) {
               auto pos = other.types.find(cur.first);
               if (pos == other.types.end()) return false;
               if (*cur.second != *pos->second) return false;
           }

           // check relations
           for(const auto& cur : relations) {
              auto pos = other.relations.find(cur.first);
              if (pos == other.relations.end()) return false;
              if (*cur.second != *pos->second) return false;
           }

           // check components
           if (!equal_targets(components, other.components)) return false;
           if (!equal_targets(instantiations, other.instantiations)) return false;
           if (!equal_targets(clauses, other.clauses)) return false;

           // no different found => programs are equal
           return true;
       }

};

} // end of namespace souffle

