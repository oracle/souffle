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
 * @file symbol_table_test.h
 *
 * Tests souffle's symbol table.
 *
 ***********************************************************************/

#include "test.h"

#include "AstProgram.h"

using namespace souffle;

namespace test {

	TEST(SymbolTable, Basics) {

	    SymbolTable a;

	    a.insert("Hello");

	    EXPECT_EQ(0, a.lookup("Hello"));
	    EXPECT_EQ(1, a.lookup("World"));

        EXPECT_STREQ("Hello", a.resolve((size_t)0));
        EXPECT_STREQ("World", a.resolve((size_t)1));

	}


    TEST(SymbolTable, Copy) {

        SymbolTable* a = new SymbolTable();
        a->insert("Hello");

        SymbolTable* b = new SymbolTable(*a);

        EXPECT_STREQ("Hello", a->resolve((size_t)0));
        EXPECT_STREQ("Hello", b->resolve((size_t)0));

        // should be different strings
        EXPECT_NE(a->resolve((size_t)0),b->resolve((size_t)0));

        // b should survive
        delete a;
        EXPECT_STREQ("Hello", b->resolve((size_t)0));

        delete b;
    }

    TEST(SymbolTable, Assign) {

        SymbolTable* a = new SymbolTable();
        a->insert("Hello");

        SymbolTable b = *a;
        SymbolTable c;

        c = *a;

        EXPECT_STREQ("Hello", a->resolve((size_t)0));
        EXPECT_STREQ("Hello", b.resolve((size_t)0));
        EXPECT_STREQ("Hello", c.resolve((size_t)0));

        // should be different strings
        EXPECT_NE(a->resolve((size_t)0),b.resolve((size_t)0));
        EXPECT_NE(a->resolve((size_t)0),c.resolve((size_t)0));
        EXPECT_NE(b.resolve((size_t)0),c.resolve((size_t)0));

        // b and c should survive
        delete a;
        EXPECT_STREQ("Hello", b.resolve((size_t)0));
        EXPECT_STREQ("Hello", c.resolve((size_t)0));

    }

} // end namespace test

