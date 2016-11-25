/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
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
#include <functional>

using namespace souffle;

namespace test {

	TEST(SymbolTable, Basics) {

	    SymbolTable table;

	    table.insert("Hello");

	    EXPECT_STREQ("Hello", table.resolve(table.lookup(table.resolve(table.lookup("Hello")))));

	    EXPECT_EQ(table.lookup("Hello"), table.lookup(table.resolve(table.lookup("Hello"))));

	    EXPECT_STREQ("Hello", table.resolve(table.lookup(table.resolve(table.lookup("Hello")))));

	    EXPECT_EQ(table.lookup("Hello"), table.lookup(table.resolve(table.lookup(table.resolve(table.lookup("Hello"))))));

	}


    TEST(SymbolTable, Copy) {

        SymbolTable* a = new SymbolTable();
        a->insert("Hello");

        SymbolTable* b = new SymbolTable(*a);

        size_t a_idx = a->lookup("Hello");
        size_t b_idx = b->lookup("Hello");

        // hash should be the same
        EXPECT_EQ(a_idx, b_idx);

        EXPECT_STREQ("Hello", a->resolve(a_idx));
        EXPECT_STREQ("Hello", b->resolve(b_idx));

        // should be different string references but the same actual string
        EXPECT_STREQ(a->resolve(a_idx), b->resolve(b_idx));
        EXPECT_NE(a->resolve(a_idx), b->resolve(b_idx));

        // b should survive
        delete a;
        EXPECT_STREQ("Hello", b->resolve(b_idx));

        delete b;
    }

    TEST(SymbolTable, Assign) {

        SymbolTable* a = new SymbolTable();
        a->insert("Hello");

        SymbolTable b = *a;
        SymbolTable c;

        c = *a;

        size_t a_idx = a->lookup("Hello");
        size_t b_idx = b.lookup("Hello");
        size_t c_idx = c.lookup("Hello");

        // hash should be the same
        EXPECT_EQ(a_idx, b_idx);
        EXPECT_EQ(b_idx, c_idx);

        EXPECT_STREQ("Hello", a->resolve(a_idx));
        EXPECT_STREQ("Hello", b.resolve(b_idx));
        EXPECT_STREQ("Hello", c.resolve(c_idx));

        // should be different strings
        EXPECT_NE(a->resolve(a_idx),b.resolve(b_idx));
        EXPECT_NE(a->resolve(a_idx),c.resolve(c_idx));
        EXPECT_NE(b.resolve(b_idx),c.resolve(c_idx));

        // b and c should survive
        delete a;
        EXPECT_STREQ("Hello", b.resolve(b_idx));
        EXPECT_STREQ("Hello", c.resolve(c_idx));

    }

    TEST(SymbolTable, Time) {

        typedef unsigned long long T;

        T n = 0;
        T N = 10000000;

        SymbolTable X;
        char* x;

        char** A = new char*[N];

        for (T i = 0; i < N; ++i) {
            x = reinterpret_cast<char*>(&i);
            time_point start = now();
            X.insert(x);
            time_point end = now();
            n += duration_in_ns(start, end);
            A[i] = x;
        }

        // time per operation for single element insert
        std::cout << n / N << " ns \n";

        // test insert for existing elements

        time_point start = now();
        X.insert((const char**) A, N);
        time_point end = now();
        n = duration_in_ns(start, end);

        // time of N element insert from char* array
        std::cout << n << " ns \n";

        // test insert for elements that don't exist yet

        SymbolTable X;

        time_point start = now();
        X.insert((const char**) A, N);
        time_point end = now();
        n = duration_in_ns(start, end);

        // time of N element insert from char* array
        std::cout << n << " ns \n";

        delete[] A;

    }


} // end namespace test

