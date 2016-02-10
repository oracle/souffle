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
 * @file AstVisitor.h
 *
 * Provides some infrastructure for the implementation of operations
 * on top of AST structures.
 *
 ***********************************************************************/

#pragma once

#include <vector>
#include <functional>

#include "AstArgument.h"
#include "AstAttribute.h"
#include "AstClause.h"
#include "AstLiteral.h"
#include "AstProgram.h"
#include "AstRelation.h"
#include "AstType.h"

/** A tag type required for the is_visitor type trait to identify AstVisitors */
struct ast_visitor_tag {};

/**
 * The generic base type of all AstVisitors realizing the dispatching of
 * visitor calls. Each visitor may define a return type R and a list of
 * extra parameters to be passed along with the visited AstNodes to the
 * corresponding visitor function.
 *
 * @tparam R the result type produced by a visit call
 * @tparam Params extra parameters to be passed to the visit call
 */
template<typename R = void, typename ... Params>
struct AstVisitor : public ast_visitor_tag {

    /** A virtual destructor */
    virtual ~AstVisitor() {}

    /** The main entry for the user allowing visitors to be utilized as functions */
    R operator()(const AstNode& node, Params ... args) {
        return visit(node, args...);
    }

    /**
     * The main entry for a visit process conducting the dispatching of
     * a visit to the various sub-types of AstNodes. Sub-classes may override
     * this implementation to conduct pre-visit operations.
     *
     * @param node the node to be visited
     * @param args a list of extra parameters to be forwarded
     */
    virtual R visit(const AstNode& node, Params ... args) {

        // dispatch node processing based on dynamic type

        #define FORWARD(Kind) \
            if (const auto* n = dynamic_cast<const Ast ## Kind *>(&node)) \
                return visit ## Kind (*n, args...);

        // types
        FORWARD(PrimitiveType);
        FORWARD(UnionType);
        FORWARD(RecordType);

        // arguments
        FORWARD(Variable)
        FORWARD(UnnamedVariable)
        FORWARD(UnaryFunctor)
        FORWARD(BinaryFunctor)
        FORWARD(Counter)
        FORWARD(NumberConstant)
        FORWARD(StringConstant)
        FORWARD(NullConstant)
        FORWARD(TypeCast)
        FORWARD(RecordInit)
        FORWARD(Aggregator)

        // literals
        FORWARD(Atom)
        FORWARD(Negation)
        FORWARD(Constraint)

        // rest
        FORWARD(Attribute);
        FORWARD(Clause);
        FORWARD(Relation);
        FORWARD(Program);

        #undef FORWARD

        // did not work ...

        std::cerr << "Unsupported type: " << typeid(node).name() << "\n";
        assert(false && "Missing Node Category!");
        return R();
    }

protected:

    #define LINK(Node,Parent) \
        virtual R visit ## Node (const Ast ## Node & n, Params ... args) { \
            return visit ## Parent ( n , args... ); \
        }

    // -- types --
    LINK(PrimitiveType, Type);
    LINK(RecordType, Type);
    LINK(UnionType, Type);
    LINK(Type, Node);

    // -- arguments --
    LINK(Variable, Argument)
    LINK(UnnamedVariable, Argument)
    LINK(Counter, Argument)
    LINK(TypeCast, Argument)
    LINK(RecordInit, Argument)

    LINK(NumberConstant, Constant)
    LINK(StringConstant, Constant)
    LINK(NullConstant, Constant)
    LINK(Constant, Argument)

    LINK(UnaryFunctor, Functor)
    LINK(BinaryFunctor, Functor)
    LINK(Functor,Argument)

    LINK(Aggregator, Argument)

    LINK(Argument, Node);


    // literals
    LINK(Atom, Literal)
    LINK(Negation, Literal)
    LINK(Constraint, Literal)
    LINK(Literal, Node);


    // -- others --
    LINK(Program, Node);
    LINK(Attribute, Node);
    LINK(Clause, Node);
    LINK(Relation, Node);

    #undef LINK

    /** The base case for all visitors -- if no more specific overload was defined */
    virtual R visitNode(const AstNode& node, Params... args) {
        return R();
    }

};

/**
 * A utility function visiting all nodes within the ast rooted by the given node
 * recursively in a depth-first pre-order fashion applying the given visitor to each
 * encountered node.
 *
 * @param root the root of the AST to be visited
 * @param visitor the visitor to be applied on each node
 * @param args a list of extra parameters to be forwarded to the visitor
 */
template<typename R, typename ... Ps, typename ... Args>
void visitDepthFirstPreOrder(const AstNode& root, AstVisitor<R,Ps...>& visitor, Args& ... args) {
    visitor(root, args...);
    for(const AstNode* cur : root.getChildNodes()) {
        visitDepthFirstPreOrder(*cur, visitor, args ...);
    }
}

/**
 * A utility function visiting all nodes within the ast rooted by the given node
 * recursively in a depth-first post-order fashion applying the given visitor to each
 * encountered node.
 *
 * @param root the root of the AST to be visited
 * @param visitor the visitor to be applied on each node
 * @param args a list of extra parameters to be forwarded to the visitor
 */
template<typename R, typename ... Ps, typename ... Args>
void visitDepthFirstPostOrder(const AstNode& root, AstVisitor<R,Ps...>& visitor, Args& ... args) {
    for(const AstNode* cur : root.getChildNodes()) {
        visitDepthFirstPreOrder(*cur, visitor, args ...);
    }
    visitor(root, args...);
}

/**
 * A utility function visiting all nodes within the ast rooted by the given node
 * recursively in a depth-first pre-order fashion applying the given visitor to each
 * encountered node.
 *
 * @param root the root of the AST to be visited
 * @param visitor the visitor to be applied on each node
 * @param args a list of extra parameters to be forwarded to the visitor
 */
template<typename R, typename ... Ps, typename ... Args>
void visitDepthFirst(const AstNode& root, AstVisitor<R,Ps...>& visitor, Args& ... args) {
    visitDepthFirstPreOrder(root, visitor, args...);
}

namespace {

    /**
     * A specialized visitor wrapping a lambda function -- an auxiliary type required
     * for visitor convenience functions.
     */
    template<typename R, typename N>
    struct LambdaVisitor : public AstVisitor<void> {
        std::function<R(const N&)> lambda;
        LambdaVisitor(const std::function<R(const N&)>& lambda) : lambda(lambda) {}
        virtual void visit(const AstNode& node) {
            if (const N* n = dynamic_cast<const N*>(&node)) {
                lambda(*n);
            }
        }
    };

    /**
     * A factory function for creating LambdaVisitor instances.
     */
    template<typename R, typename N>
    LambdaVisitor<R,N> makeLambdaVisitor(const std::function<R(const N&)>& fun) {
        return LambdaVisitor<R,N>(fun);
    }

    /**
     * A type trait determining whether a given type is a visitor or not.
     */
    template<typename T>
    struct is_visitor {
        enum { value = std::is_base_of<ast_visitor_tag,T>::value };
    };

    template<typename T>
    struct is_visitor<const T> : public is_visitor<T> {};

    template<typename T>
    struct is_visitor<T&> : public is_visitor<T> {};
}

/**
 * A utility function visiting all nodes within the ast rooted by the given node
 * recursively in a depth-first pre-order fashion applying the given function to each
 * encountered node.
 *
 * @param root the root of the AST to be visited
 * @param fun the function to be applied
 * @param args a list of extra parameters to be forwarded to the visitor
 */
template<typename R, typename N>
void visitDepthFirst(const AstNode& root, const std::function<R(const N&)>& fun) {
    auto visitor = makeLambdaVisitor(fun);
    visitDepthFirst<void>(root, visitor);
}

/**
 * A utility function visiting all nodes within the ast rooted by the given node
 * recursively in a depth-first pre-order fashion applying the given function to each
 * encountered node.
 *
 * @param root the root of the AST to be visited
 * @param fun the function to be applied
 * @param args a list of extra parameters to be forwarded to the visitor
 */
template<
    typename Lambda,
    typename R = typename lambda_traits<Lambda>::result_type,
    typename N = typename lambda_traits<Lambda>::arg0_type
>
typename std::enable_if<!is_visitor<Lambda>::value ,void>::type
visitDepthFirst(const AstNode& root, const Lambda& fun) {
    visitDepthFirst(root, std::function<R(const N&)>(fun));
}

/**
 * A utility function visiting all nodes within a given list of AST root nodes
 * recursively in a depth-first pre-order fashion applying the given function to each
 * encountered node.
 *
 * @param list the list of roots of the ASTs to be visited
 * @param fun the function to be applied
 * @param args a list of extra parameters to be forwarded to the visitor
 */
template<typename T, typename Lambda>
void visitDepthFirst(const std::vector<T*>& list, const Lambda& fun) {
    for(const auto& cur : list) {
        visitDepthFirst(*cur, fun);
    }
}

/**
 * A utility function visiting all nodes within a given list of AST root nodes
 * recursively in a depth-first pre-order fashion applying the given function to each
 * encountered node.
 *
 * @param list the list of roots of the ASTs to be visited
 * @param fun the function to be applied
 * @param args a list of extra parameters to be forwarded to the visitor
 */
template<typename T, typename Lambda>
void visitDepthFirst(const std::vector<std::unique_ptr<T>>& list, const Lambda& fun) {
    for(const auto& cur : list) {
        visitDepthFirst(*cur, fun);
    }
}

/**
 * A utility function visiting all nodes within the ast rooted by the given node
 * recursively in a depth-first post-order fashion applying the given function to each
 * encountered node.
 *
 * @param root the root of the AST to be visited
 * @param fun the function to be applied
 * @param args a list of extra parameters to be forwarded to the visitor
 */
template<typename R, typename N>
void visitDepthFirstPostOrder(const AstNode& root, const std::function<R(const N&)>& fun) {
    auto visitor = makeLambdaVisitor(fun);
    visitDepthFirstPostOrder<void>(root, visitor);
}

/**
 * A utility function visiting all nodes within the ast rooted by the given node
 * recursively in a depth-first post-order fashion applying the given function to each
 * encountered node.
 *
 * @param root the root of the AST to be visited
 * @param fun the function to be applied
 * @param args a list of extra parameters to be forwarded to the visitor
 */
template<
    typename Lambda,
    typename R = typename lambda_traits<Lambda>::result_type,
    typename N = typename lambda_traits<Lambda>::arg0_type
>
typename std::enable_if<!is_visitor<Lambda>::value ,void>::type
visitDepthFirstPostOrder(const AstNode& root, const Lambda& fun) {
    visitDepthFirstPostOrder(root, std::function<R(const N&)>(fun));
}

