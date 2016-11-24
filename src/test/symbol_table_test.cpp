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
#include <chrono>

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

    TEST(SymbolTable, Time) {

        unsigned long long totalTime = 0;
        unsigned int numberOfRuns = 10;
        unsigned long operationsPerRun = 1000000;

        for (unsigned int i = 0; i < numberOfRuns; ++i) {

            SymbolTable a;

            // start the timer
            time_point start = now();

            for (unsigned long i = 0; i < operationsPerRun; ++i) {
                a.insert(reinterpret_cast<const char*>(&i));
            }

            // stop the timer
            time_point end = now();

            totalTime += duration_in_ns(start, end);
        }

        long averageTime = totalTime / numberOfRuns;

        std::cout << averageTime << " ns \n";

    }

} // end namespace test

