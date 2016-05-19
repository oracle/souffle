/*
 * Souffle version 0.0.0
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file type_system_test.h
 *
 * Tests souffle's type system operations.
 *
 ***********************************************************************/

#include "test.h"

#include "RuleScheduler.h"

namespace souffle {
namespace test {

    using namespace scheduler;

    TEST(Scheduler, SimpleProblem_101) {

        typedef Problem<SimpleComputationalCostModel> Problem;
        typedef SimpleComputationalCostAtom Atom;

        Argument x = Argument::createVar(1);
        Argument y = Argument::createVar(2);
        Argument z = Argument::createVar(3);

        Atom a(1, toVector(x,y), 50);
        Atom b(2, toVector(y,z), 20);

        Problem p(toVector(a,b));

        EXPECT_EQ(toVector(b,a), p.solve())
            << "Problem: " << p << "\n";
    }

    TEST(Scheduler, SimpleProblem_102) {

        typedef Problem<SimpleComputationalCostModel> Problem;
        typedef SimpleComputationalCostAtom Atom;

        Argument x = Argument::createVar(1);
        Argument y = Argument::createVar(2);
        Argument z = Argument::createVar(3);
        Argument w = Argument::createVar(4);

        Atom a(1, toVector(x,y), 80);
        Atom b(2, toVector(y,z), 50);
        Atom c(3, toVector(z,w), 20);

        Problem p(toVector(a,b,c));

        EXPECT_EQ(toVector(c,b,a), p.solve())
            << "Problem: " << p << "\n";
    }

    TEST(Optimizer, RealWorldExample_1) {

        typedef Problem<SimpleComputationalCostModel> Problem;
        typedef SimpleComputationalCostAtom Atom;

        Argument x = Argument::createVar(1);
        Argument y = Argument::createVar(2);
        Argument z = Argument::createVar(3);
        Argument w = Argument::createVar(4);

        Atom a(1, toVector(x,y), 1705);
        Atom b(2, toVector(x,z), 21254);
        Atom c(3, toVector(y,w), 50851);

        Problem p(toVector(a,b,c));

        EXPECT_EQ(toVector(a,b,c), p.solve())
            << "Problem: " << p << "\n";

    }

    TEST(Optimizer, RealWorldExample_2) {

        typedef Problem<SimpleComputationalCostModel> Problem;
        typedef SimpleComputationalCostAtom Atom;

        // { <0>|2154|( 0,1 ), <1>|1046|( 2 ), <2>|14691|( 2,0 ), <3>|245705625|( 3,2 ) }

        Argument x = Argument::createVar(1);
        Argument y = Argument::createVar(2);
        Argument z = Argument::createVar(3);
        Argument w = Argument::createVar(4);


        Atom a(1, toVector(x,y), 2154);
        Atom b(2, toVector(z),   1046);
        Atom c(3, toVector(z,x), 14691);
        Atom d(4, toVector(w,y), 245705625);

        Problem p(toVector(a,b,c,d));

        EXPECT_EQ(toVector(b,c,a,d), p.solve())
            << "Problem: " << p << "\n";

    }


    TEST(Optimizer, Scalability) {

        typedef Problem<SimpleComputationalCostModel> Problem;
        typedef SimpleComputationalCostAtom Atom;

        Argument x = Argument::createVar(1);
        Argument y = Argument::createVar(2);

        Problem p;
        for(int i=0; i<10; i++) {
            auto start = now();
            p.solve();
            auto end = now();
            std::cout << "Solving " << i << " took " << duration_in_ms(start, end) << "ms\n";
            p.addAtom(Atom((i+1)*10, toVector(x,y), 123));
        }

    }

} // end namespace test
} // end namespace souffle

