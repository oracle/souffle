/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file RamOperation.h
 *
 * Defines the Operation of a relational algebra query. 
 *
 ***********************************************************************/

#pragma once

#include <set>

#include "RamNode.h"
#include "RamRelation.h"
#include "RamCondition.h"
#include "RamIndex.h"

namespace souffle {

/** Abstract class for a relational algebra operation */ 
class RamOperation : public RamNode {
protected:

    /** the nesting level of this operation */
    size_t level; 

    /** conditions that is checked for each obtained tuple */
    std::unique_ptr<RamCondition> condition;

public:

    RamOperation(RamNodeType type, size_t l)
        : RamNode(type), level(l), condition(nullptr) {}

    virtual ~RamOperation() { }

    /** obtains the level of this operation */
    size_t getLevel() const { return level; }

    /** get depth of query */ 
    virtual size_t getDepth() const = 0;

    /** Pretty print output to a given output stream */
    virtual void print(std::ostream &os, int tabpos) const = 0;

    /** Pretty print node */
    virtual void print(std::ostream& os) const { print(os, 0); }

    /** Add condition */
    virtual void addCondition(std::unique_ptr<RamCondition> c, RamOperation *root);

    /** Add condition */
    void addCondition(std::unique_ptr<RamCondition> c) {
        addCondition(std::move(c), this);
    }

    /** get the optional condition on this level */
    RamCondition* getCondition() const {
        return condition.get();
    }

    /** Obtains a list of child nodes */
    virtual std::vector<const RamNode*> getChildNodes() const {
        if (!condition) return toVector<const RamNode*>();
        return { condition.get() };
    }
};

/** Generic super type for scans and lookups */
class RamSearch : public RamOperation {

    /** nested operation */
    std::unique_ptr<RamOperation> nestedOperation;

public:

    RamSearch(RamNodeType type, std::unique_ptr<RamOperation> nested)
        : RamOperation(type, nested->getLevel() - 1), nestedOperation(std::move(nested)) {}

    ~RamSearch() { }

    /** get nested operation */
    RamOperation* getNestedOperation() const {
        return nestedOperation.get();
    }

    /** set nested operation */
    void setNestedOperation(std::unique_ptr<RamOperation> o) {
        nestedOperation.swap(o);
    }

    /** Add condition */
    void addCondition(std::unique_ptr<RamCondition> c, RamOperation *root);

    /** get depth of query */
    virtual size_t getDepth() const {
        return 1+nestedOperation->getDepth();
    }

    /** Obtains a list of child nodes */
    virtual std::vector<const RamNode*> getChildNodes() const {
        auto res = RamOperation::getChildNodes();
        res.push_back(nestedOperation.get());
        return res;
    }
};


/** iterates of a table and checks conditions */ 
class RamScan : public RamSearch {
protected: 

    /** the targeted relation */
    RamRelationIdentifier relation;

    /** Values of index per column of table (if indexable) */ 
    std::vector<std::unique_ptr<RamValue>> queryPattern;

    /** the columns to be matched when using a range query */
    SearchColumns keys;

    /**
     * Determines whether this scan operation is merely verifying the existence
     * of a value (e.g. rel(_,_), rel(1,2), rel(1,_) or rel(X,Y) where X and Y are bound)
     * or actually contributing new variable bindings (X or Y are not bound).
     *
     * The exists-only can be check much more efficient than the other case.
     */
    bool pureExistenceCheck;

    /** A reference to the utilized index */
    mutable RamIndex* index;

public:

    /** constructs a scan operation on the given relation with the given nested operation */
    RamScan(const RamRelationIdentifier& r, std::unique_ptr<RamOperation> nested, bool pureExistenceCheck)
        : RamSearch(RN_Scan, std::move(nested)), relation(r), queryPattern(r.getArity()),
          keys(0), pureExistenceCheck(pureExistenceCheck), index(nullptr) {}

    ~RamScan() { }

    /** Obtains the id of the relation scanned by this operation */
    const RamRelationIdentifier& getRelation() const {
        return relation;
    }

    /**
     * Obtains a mask indicating the keys to be matched when realizing this scan
     * via a range query.
     */
    SearchColumns getRangeQueryColumns() const {
        return keys;
    }

    /**
     * Obtains the pattern of values to be utilized as the input for a
     * range query.
     */
    std::vector<RamValue*> getRangePattern() const {
        return toPtrVector(queryPattern);
    }

    /** Determines whether this scan step is merely checking the existence of some value */
    bool isPureExistenceCheck() const {
        return pureExistenceCheck;
    }

    /** Marks this scan step as a pure existence check or not */
    void setPureExistenceCheck(bool value = true) {
        pureExistenceCheck = value;
    }

    /** Obtains the index utilized by this operation */
    RamIndex* getIndex() const {
        return index;
    }

    /** updates the index utilized by this operation */
    void setIndex(RamIndex* index) const {
        this->index = index;
    }

    /** print search */ 
    void print(std::ostream &os, int tabpos) const;

    /** add condition */
    virtual void addCondition(std::unique_ptr<RamCondition> c, RamOperation *root);

    /** Obtains a list of child nodes */
    virtual std::vector<const RamNode*> getChildNodes() const {
        auto res = RamSearch::getChildNodes();
        for(auto &cur : queryPattern) {
            if (cur) res.push_back(cur.get());
        }
        return res;
    }

};


/** Lookup of records */
class RamLookup : public RamSearch {

    /** The level of the tuple containing the reference to resolve */
    std::size_t refLevel;

    /** The position of the tuple reference in the tuple on the corresponding level */
    std::size_t refPos;

    /** The arity of the unpacked tuple */
    std::size_t arity;

public:

    RamLookup(std::unique_ptr<RamOperation> nested, std::size_t ref_level, std::size_t ref_pos, std::size_t arity)
        : RamSearch(RN_Lookup, std::move(nested)), refLevel(ref_level), refPos(ref_pos), arity(arity) {}

    std::size_t getReferenceLevel() const {
        return refLevel;
    }

    std::size_t getReferencePosition() const {
        return refPos;
    }

    std::size_t getArity() const {
        return arity;
    }

    /** print search */
    void print(std::ostream &os, int tabpos) const;

};


/** A ram aggregation is computing an aggregated value over a given relation */
class RamAggregate : public RamSearch {

public:

    /** An enumeration of supported aggregation functions */
    enum Function {
        MAX,
        MIN,
        COUNT
    };

private:

    /** A aggregation function performed */
    Function fun;

    /** The value to be aggregated */
    std::unique_ptr<RamValue> value;

    /** The relation to be scanned */
    RamRelationIdentifier relation;

    /** The pattern for filtering relevant tuples */
    std::vector<std::unique_ptr<RamValue>> pattern;

    /** the columns to be matched when using a range query */
    SearchColumns keys;

    /** A reference to the utilized index */
    mutable RamIndex* index;

public:

    /** Creates a new instance based on the given parameters */
    RamAggregate(std::unique_ptr<RamOperation> nested, Function fun, std::unique_ptr<RamValue> value, const RamRelationIdentifier& relation)
        : RamSearch(RN_Aggregate, std::move(nested)), fun(fun), value(std::move(value)), relation(relation), pattern(relation.getArity()), keys(0), index(nullptr) {
    }

    ~RamAggregate() { }

    Function getFunction() const {
        return fun;
    }

    const RamValue* getTargetExpression() const {
        return value.get();
    }

    const RamRelationIdentifier& getRelation() const {
        return relation;
    }

    std::vector<RamValue*> getPattern() const {
        return toPtrVector(pattern);
    }

    /** add condition */
    virtual void addCondition(std::unique_ptr<RamCondition> c, RamOperation *root);

    /**
     * Obtains a mask indicating the keys to be matched when realizing this scan
     * via a range query.
     */
    SearchColumns getRangeQueryColumns() const {
        return keys;
    }

    /** Obtains the index utilized by this operation */
    RamIndex* getIndex() const {
        return index;
    }

    /** updates the index utilized by this operation */
    void setIndex(RamIndex* index) const {
        this->index = index;
    }

    /** print search */
    void print(std::ostream &os, int tabpos) const;

};


/** Projection */ 
class RamProject: public RamOperation {
protected: 

    /** relation */ 
    RamRelationIdentifier relation;

    /** a relation to check that the projected value is not present */
    std::unique_ptr<RamRelationIdentifier> filter;

    /* values for projection */ 
    std::vector<std::unique_ptr<RamValue>> values;

public:

    RamProject(const RamRelationIdentifier& relation, size_t level)
        : RamOperation(RN_Project, level), relation(relation), filter(nullptr) {}

    RamProject(const RamRelationIdentifier& relation, const RamRelationIdentifier& filter, size_t level)
        : RamOperation(RN_Project, level), relation(relation), filter(std::unique_ptr<RamRelationIdentifier>(new RamRelationIdentifier(filter))) {}

    ~RamProject() { }

    /** add value for a column */ 
    void addArg(std::unique_ptr<RamValue> v) {
        values.push_back(std::move(v));
    }

    const RamRelationIdentifier& getRelation() const {
        return relation;
    }

    bool hasFilter() const {
        return (bool) filter;
    }

    const RamRelationIdentifier& getFilter() const {
        assert(hasFilter());
        return *filter;
    }

    std::vector<RamValue*> getValues() const {
        return toPtrVector(values);
    }

    /** get depth of projection */ 
    virtual size_t getDepth() const {
        return 1;
    }

    /** execute print */ 
    virtual void print(std::ostream &os, int tabpos) const;

    /** Obtains a list of child nodes */
    virtual std::vector<const RamNode*> getChildNodes() const {
        auto res = RamOperation::getChildNodes();
        for(const auto& cur : values) res.push_back(cur.get());
        return res;
    }
};

} // end of namespace souffle

