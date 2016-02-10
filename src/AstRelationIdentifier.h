/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All Rights reserved
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
 * @file AstRelationIdentifier.h
 *
 * Defines the token utilized to address relations.
 *
 ***********************************************************************/

#pragma once

#include <algorithm>
#include <string>
#include <vector>

#include "Util.h"

/**
 * The type of identifier utilized for referencing relations. Relation
 * name identifiers are hierarchically qualified names, e.g.
 *
 *          problem.graph.edge
 *
 */
class AstRelationIdentifier {

    /**
     * The list of names forming this identifier.
     */
    std::vector<std::string> names;

public:

    // -- constructors --

    AstRelationIdentifier(const std::string& name = "")
        : names(toVector(name)) {}

    AstRelationIdentifier(const char* name)
        : AstRelationIdentifier(std::string(name)) {}

    AstRelationIdentifier(const AstRelationIdentifier&) =default;
    AstRelationIdentifier(AstRelationIdentifier&&) =default;


    // -- assignment operators --

    AstRelationIdentifier& operator=(const AstRelationIdentifier&) = default;
    AstRelationIdentifier& operator=(AstRelationIdentifier&&) = default;


    // -- mutators --

    void append(const std::string& name) {
        names.push_back(name);
    }

    void prepent(const std::string& name) {
        names.insert(names.begin(), name);
    }


    // -- getters and setters --

    const std::vector<std::string>& getNames() const {
        return names;
    }


    // -- comparison operators --

    bool operator==(const AstRelationIdentifier& other) const {
        return names == other.names;
    }

    bool operator!=(const AstRelationIdentifier& other) const {
        return !(*this == other);
    }

    bool operator<(const AstRelationIdentifier& other) const {
        return std::lexicographical_compare(names.begin(), names.end(), other.names.begin(), other.names.end());
    }

    void print(std::ostream& out) const {
        out << join(names, ".");
    }

    friend std::ostream& operator<<(std::ostream& out, const AstRelationIdentifier& id) {
        id.print(out);
        return out;
    }

};

/**
 * A overloaded operator to add a new prefix to a given relation identifier.
 */
inline AstRelationIdentifier operator+(const std::string& name, const AstRelationIdentifier& id) {
    AstRelationIdentifier res = id;
    res.prepent(name);
    return res;
}
