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
 * @file AstSrcLocation.h
 *
 * Structures to describe the location of AST nodes within input code.
 *
 ***********************************************************************/

#pragma once

#include <string>
#include <ostream>

namespace souffle {

/** A class describing a range in an input file */
class AstSrcLocation {
public:

    /** A class locating a single point in an input file */
    struct Point {

        /** The line in the source file */
        int line;

        /** The column in the source file */
        int column;

        /** A comparison for points */
        bool operator<(const Point& other) const {
            return line < other.line || (line == other.line && column < other.column);
        }

        bool operator>(const Point& other) const {
            return other < *this;
        }

        void print(std::ostream& out) const {
            out << line << ":" << column;

        }

        /** Enables locations to be printed */
        friend std::ostream& operator<<(std::ostream& out, const Point& loc) {
            loc.print(out);
            return out;
        }
    };

    /** The file referred to */
    std::string filename;

    /** The start location */
    Point start;

    /** The End location */
    Point end;


    /** A comparison for source locations */
    bool operator<(const AstSrcLocation& other) const {
        if (filename < other.filename) return true;
        if (filename > other.filename) return false;
        if (start < other.start) return true;
        if (start > other.start) return false;
        if (end < other.end) return true;
        return false;
    }

    /** An extended string describing this location in a end-user friendly way */
    std::string extloc() const;

    void print(std::ostream& out) const {
        out << filename << " [" << start << "-" << end << "]";
    }

    /** Enables ranges to be printed */
    friend std::ostream& operator<<(std::ostream& out, const AstSrcLocation& range) {
        range.print(out);
        return out;
    }

};

} // end of namespace souffle

