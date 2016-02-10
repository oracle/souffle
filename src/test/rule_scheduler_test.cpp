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
 * @file type_system_test.h
 *
 * Tests souffle's type system operations.
 *
 ***********************************************************************/

#include "test.h"

#include "RuleScheduler.h"

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

