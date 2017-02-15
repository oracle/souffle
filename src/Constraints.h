/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file Constraints.h
 *
 * A basic set of utilities for solving systems of constraints.
 *
 ***********************************************************************/

#pragma once

#include "Util.h"

#include <iostream>
#include <memory>
#include <set>
#include <vector>

namespace souffle {

//----------------------------------------------------------------------
//                      forward declarations
//----------------------------------------------------------------------

template <typename Id, typename PropertySpace>
struct Variable;

template <typename Var>
class Constraint;

template <typename Var>
class Assignment;

template <typename Var>
class Problem;

//----------------------------------------------------------------------
//                  property space constructors
//----------------------------------------------------------------------

namespace detail {

template <typename T>
struct default_bottom_factory {
    T operator()() const {
        return T();
    }
};

template <typename T, typename meet_assign_op>
struct default_meet_op {
    T operator()(const T& a, const T& b) {
        T res = a;
        meet_assign_op()(res, b);
        return res;
    }
};
}

/**
 * A MPL type for defining a property space. A property space consists of
 * a value type T (the domain of the property space), a meet operator and
 * a factory for computing the bottom value of the property space.
 *
 * For performance reasons the meet operator is defined by a meet-assign
 * operator mutating the first operator to become the actual result of the
 * meet operation.
 *
 * @tparam T the value domain of the defined property space
 * @tparam meet_assign_op the meet-assign operator defining the lattice over the
 *          values of T forming the foundation for the resulting property space
 * @tparam bottom_factory a functor producing the bottom element of the property space
 *          by default the default constructor of T will be utilized
 * @tparam meet_op a non destructive meet operator, by default derived from the
 *          meet-assign operator
 */
template <typename T, typename meet_assign_op,
        typename bottom_factory = typename detail::default_bottom_factory<T>,
        typename meet_op = typename detail::default_meet_op<T, meet_assign_op>>
struct property_space {
    typedef T value_type;
    typedef meet_assign_op meet_assign_op_type;
    typedef meet_op meet_op_type;
    typedef bottom_factory bottom_factory_type;
};

namespace detail {

/**
 * A meet operator for set-based property spaces based on the sub-set lattices.
 */
template <typename T>
struct set_meet_assign_op {
    bool operator()(std::set<T>& a, const std::set<T>& b) {
        bool changed = false;
        for (const auto& cur : b) {
            changed = a.insert(cur).second || changed;
        }
        return changed;
    }
};
}

/**
 * A property space for set-based properties based on sub-set lattices.
 */
template <typename T>
struct set_property_space : public property_space<std::set<T>, detail::set_meet_assign_op<T>> {};

//----------------------------------------------------------------------
//                           variables
//----------------------------------------------------------------------

/**
 * A variable to be utilized within constraints to be handled by the
 * constraint solver.
 *
 * @tparam Id the type of object this variable shall be bound to
 * @tparam PropertySpace the property space this variable is associated to
 */
template <typename Id, typename PropertySpace>
struct Variable {
    /** exports the property space */
    typedef PropertySpace property_space;

protected:
    /** the underlying value giving this variable its identity */
    Id id;

public:
    Variable(const Id& id) : id(id) {}
    virtual ~Variable() {}

    Variable(const Variable&) = default;
    Variable(Variable&&) = default;

    Variable& operator=(const Variable&) = default;
    Variable& operator=(Variable&&) = default;

    /** Adds support for equality comparison */
    bool operator==(const Variable& other) const {
        return id == other.id;
    }

    /** Adds support for inequality comparison */
    bool operator!=(const Variable& other) const {
        return !(*this == other);
    }

    /** Adds support for less-than comparison */
    bool operator<(const Variable& other) const {
        return id < other.id;
    }

    /** Adds print support */
    virtual void print(std::ostream& out) const {
        out << id;
    }

    /** Adds print support */
    friend std::ostream& operator<<(std::ostream& out, const Variable& var) {
        var.print(out);
        return out;
    }
};

//----------------------------------------------------------------------
//                          constraints
//----------------------------------------------------------------------

/**
 * A generic base class for constraints on variables.
 *
 * @tparam Var the type of variables constraint.
 */
template <typename Var>
class Constraint {
    typedef typename Var::property_space property_space;

public:
    /** A virtual destructor */
    virtual ~Constraint() {}

    /**
     * Requests the given assignment to be updated according to
     * this constraint.
     *
     * @param ass the assignment to be updated
     * @return true if the assignment was altered, false otherwise
     */
    virtual bool update(Assignment<Var>& ass) const = 0;

    /** Adds print support for constraints (debugging) */
    virtual void print(std::ostream& out) const = 0;

    /** Adds print support for constraints (debugging) */
    friend std::ostream& operator<<(std::ostream& out, const Constraint& c) {
        c.print(out);
        return out;
    }
};

//----------------------------------------------------------------------
//                    generic constraint factories
//----------------------------------------------------------------------

/**
 * A generic factory for constraints of the form
 *
 *                                a ⊑ b
 *
 * where a and b are variables and ⊑ is the order relation induced by
 * their associated property space.
 */
template <typename Var>
std::shared_ptr<Constraint<Var>> sub(const Var& a, const Var& b, const std::string& symbol = "⊑") {
    struct Sub : public Constraint<Var> {
        Var a, b;
        std::string symbol;

        Sub(const Var& a, const Var& b, const std::string& symbol) : a(a), b(b), symbol(symbol) {}

        virtual bool update(Assignment<Var>& ass) const {
            typename Var::property_space::meet_assign_op_type meet_assign;
            return meet_assign(ass[b], ass[a]);
        }

        virtual void print(std::ostream& out) const {
            out << a << " " << symbol << " " << b;
        }
    };

    return std::make_shared<Sub>(a, b, symbol);
}

/**
 * A generic factory for constraints of the form
 *
 *                                a ⊑ b
 *
 * where b is a variables, a is a value of b's property space, and ⊑ is
 * the order relation induced by b's property space.
 */
template <typename Var, typename Val = typename Var::property_space::value_type>
std::shared_ptr<Constraint<Var>> sub(const Val& a, const Var& b, const std::string& symbol = "⊑") {
    struct Sub : public Constraint<Var> {
        Val a;
        Var b;
        std::string symbol;

        Sub(const Val& a, const Var& b, const std::string& symbol) : a(a), b(b), symbol(symbol) {}

        virtual bool update(Assignment<Var>& ass) const {
            typename Var::property_space::meet_assign_op_type meet_assign;
            return meet_assign(ass[b], a);
        }

        virtual void print(std::ostream& out) const {
            out << a << " " << symbol << " " << b;
        }
    };

    return std::make_shared<Sub>(a, b, symbol);
}

//----------------------------------------------------------------------
//                           assignment
//----------------------------------------------------------------------

/**
 * An assignment maps a list of variables to values of their respective
 * property space.
 *
 * @tparam Var the kind of variable forming the domain of this assignment
 */
template <typename Var>
class Assignment {
    // a few type definitions
    typedef typename Var::property_space property_space;
    typedef typename property_space::value_type value_type;
    typedef typename property_space::bottom_factory_type bottom_factory_type;

    typedef typename std::map<Var, value_type> data_type;

    /** a copy of the value assigned to all unmapped variables */
    value_type bottom;

    /** the actual mapping of variables to values */
    data_type data;

public:
    typedef typename data_type::const_iterator iterator;

    /** Creates a new, empty assignment */
    Assignment() : bottom(bottom_factory_type()()) {}

    /**
     * Looks up the value associated to the given variable. Every
     * Assignment is a total mapping assigning each variable in
     * the domain of type {Var} a value of its property space. If
     * not defined earlier, it will be the bottom value.
     *
     * @param var the variable whose value is required
     * @return a const reference to the associated value
     */
    const value_type& operator[](const Var& var) const {
        auto pos = data.find(var);
        return (pos != data.end()) ? pos->second : bottom;
    }

    /**
     * Looks up the value associated to the given variable. Every
     * Assignment is a total mapping assigning each variable in
     * the domain of type {Var} a value of its property space. If
     * not defined earlier, it will be bound to the bottom value.
     *
     * @param var the variable whose value is required
     * @return a mutable reference to the associated value
     */
    value_type& operator[](const Var& var) {
        auto pos = data.find(var);
        if (pos == data.end()) {
            return data[var] = bottom;
        }
        return pos->second;
    }

    /** Adds print support */
    void print(std::ostream& out) const {
        out << data;
    }

    /** Adds print support */
    friend std::ostream& operator<<(std::ostream& out, const Assignment& ass) {
        ass.print(out);
        return out;
    }

    /** Allows to iterate over the maplets defining this assignment. */
    iterator begin() const {
        return data.begin();
    }

    /** Allows to iterate over the maplets defining this assignment. */
    iterator end() const {
        return data.end();
    }
};

//----------------------------------------------------------------------
//                        problem & solver
//----------------------------------------------------------------------

/**
 * A problem is a list of constraints for which a solution is desired.
 *
 * @tparam Var the domain of variables handled by this problem
 */
template <typename Var>
class Problem {
    // a few type definitions
    typedef Constraint<Var> constraint;
    typedef std::shared_ptr<constraint> constraint_ptr;

    /** The list of covered constraints */
    std::vector<constraint_ptr> constraints;

public:
    /**
     * Adds another constraint to the internally maintained list of constraints.
     */
    void add(const constraint_ptr& constraint) {
        constraints.push_back(constraint);
    }

    /**
     * Computes a solution (minimum fixpoint) for the contained list of
     * constraints.
     *
     * @return an assignment representing a solution for this problem
     */
    Assignment<Var> solve() const {
        Assignment<Var> res;
        return solve(res);
    }

    /**
     * Computes a solution (minimum fixpoint) for the contained list of
     * constraints based on an initial assignment.
     *
     * @return an assignment representing a solution for this problem
     */
    Assignment<Var>& solve(Assignment<Var>& ass) const {
        // this is the most naive version of a solver, but sound and complete
        bool change = true;
        while (change) {
            change = false;
            for (const auto& cur : constraints) {
                change = cur->update(ass) || change;
            }
        }

        // already done
        return ass;
    }

    /** Enables a problem to be printed (debugging) */
    void print(std::ostream& out) const {
        if (constraints.empty()) {
            out << "{}";
        } else {
            out << "{\n\t" << join(constraints, ",\n\t", print_deref<constraint_ptr>()) << "\n}";
        }
    }

    friend std::ostream& operator<<(std::ostream& out, const Problem& p) {
        p.print(out);
        return out;
    }
};

}  // end of namespace souffle
