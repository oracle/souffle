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
 * @file AstNode.h
 *
 * Top level syntactic element of intermediate representation,
 * i.e., a node of abstract syntax tree
 *
 ***********************************************************************/

#pragma once

#include <stddef.h>
#include <unistd.h>
#include <libgen.h>
#include <string.h>
#include <memory>

#include <limits>
#include <typeinfo>

#include "Util.h"
#include "AstSrcLocation.h"

namespace souffle {

// forward declaration -- see below
class AstNodeMapper;

/**
 *  @class AstNode
 *  @brief AstNode is a superclass for all elements of IR that
 *         correspond to syntactic elements of a datalog program.
 */
class AstNode {

    /** Source location of a syntactic element */
    AstSrcLocation location;

public:

    virtual ~AstNode() {}

    /** Return source location of the AstNode */
    AstSrcLocation getSrcLoc() const { return location; }

    /** Set source location for the AstNode */
    void setSrcLoc(const AstSrcLocation& l) { location = l; }

    /** Return extended location associated with this
      AstNode (redirect from SrcLoc). */
    std::string extloc() const {
        return location.extloc();
    }

    /** Enables the comparison of AST nodes */
    bool operator==(const AstNode& other) const {
        return this == &other || (typeid(*this) == typeid(other) && equal(other));
    }

    /** Enables the comparison of AST nodes */
    bool operator!=(const AstNode& other) const {
        return !(*this == other);
    }

    /** Requests an independent, deep copy of this node */
    virtual AstNode* clone() const =0;

    /** Applies the node mapper to all child nodes and conducts the corresponding replacements */
    virtual void apply(const AstNodeMapper& mapper) =0;

    /** Obtains a list of all embedded child nodes */
    virtual std::vector<const AstNode*> getChildNodes() const =0;

    /** Output to a given output stream */
    virtual void print(std::ostream &os) const = 0;

    /** Enables all nodes to be printed to some output stream */
    friend std::ostream& operator<<(std::ostream& out, const AstNode& node) {
    	node.print(out);
    	return out;
    }

protected:

    /** An internal function to determine equality to another node */
    virtual bool equal(const AstNode& other) const =0;

};


/**
 * An abstract base class for AST node manipulation operations mapping
 * a AST nodes to substitutions.
 */
class AstNodeMapper {
public:

    /** A virtual destructor for this abstract type */
    virtual ~AstNodeMapper() {}

    /**
     * Computes a replacement for the given node. If the given nodes
     * is to be replaced, the handed in node will be destroyed by the mapper
     * and the returned node will become owned by the caller.
     */
    virtual std::unique_ptr<AstNode> operator()(std::unique_ptr<AstNode> node) const =0;

    /**
     * A generic wrapper over the map function above to avoid unnecessary
     * casting operations.
     */
    template<typename T>
    std::unique_ptr<T> operator()(std::unique_ptr<T> node) const {
        std::unique_ptr<AstNode> resPtr = (*this)(std::unique_ptr<AstNode>(static_cast<AstNode*>(node.release())));
        assert(dynamic_cast<T*>(resPtr.get()) && "Invalid target node!");
        return std::unique_ptr<T>(dynamic_cast<T*>(resPtr.release()));
    }
};

namespace detail {

    /**
     * A special AstNodeMapper wrapping a lambda conducting node transformations.
     */
    template<typename Lambda>
    class LambdaNodeMapper : public AstNodeMapper {

        const Lambda& lambda;

    public:

        LambdaNodeMapper(const Lambda& lambda) : lambda(lambda) {}

        virtual std::unique_ptr<AstNode> operator()(std::unique_ptr<AstNode> node) const {
            return lambda(std::move(node));
        }
    };

}

/**
 * Creates a node mapper based on a corresponding lambda expression.
 */
template<typename Lambda>
detail::LambdaNodeMapper<Lambda> makeLambdaMapper(const Lambda& lambda) {
    return detail::LambdaNodeMapper<Lambda>(lambda);
}

} // end of namespace souffle

