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
 * @file ram_relation_stats_test.h
 *
 * Tests for the ram relation statistics extraction utility.
 *
 ***********************************************************************/

#include "test.h"

#include <fstream>

#include "RamRelationStats.h"

using namespace souffle;

namespace test {

    TEST(Stats, Basic) {

        // create a table
        RamRelationIdentifier id("a",3);
        RamRelation rel(id);

        // add some values
        rel.insert(1,1,1);
        rel.insert(1,2,1);
        rel.insert(1,3,2);
        rel.insert(1,4,2);

        RamRelationStats stats = RamRelationStats::extractFrom(rel);


        EXPECT_EQ(1, stats.getEstimatedCardinality(0));
        EXPECT_EQ(4, stats.getEstimatedCardinality(1));
        EXPECT_EQ(2, stats.getEstimatedCardinality(2));

    }

    TEST(Stats, Function) {

        // create a table
        RamRelationIdentifier id("a",2);
        RamRelation rel(id);

        // add some values
        for(int i=0; i<10000; i++) {
            rel.insert(i, i % 5);
        }

        RamRelationStats stats = RamRelationStats::extractFrom(rel, 100);

        EXPECT_EQ(100, stats.getSampleSize());
        EXPECT_EQ(10000, stats.getCardinality());

        EXPECT_EQ(10000, stats.getEstimatedCardinality(0));
        EXPECT_EQ(500, stats.getEstimatedCardinality(1));

    }

    TEST(Stats, Convergence) {

        // load a table
        RamRelationIdentifier id("a", 2);
        RamRelation rel(id);

        SymbolTable symTable;

        SymbolMask mask(2);
        mask.setSymbol(0);
        mask.setSymbol(1);

        {
            std::fstream in(BUILDDIR "../tests/evaluation/hmmer/facts/DirectFlow.facts");

            // if file not found => be done
            if (!in.is_open()) return;

            rel.load(in, symTable, mask);
        }

        std::cout << rel.size() << "\n";


        RamRelationIdentifier id2("b", 3);
        RamRelation rel2(id2);

        for(const auto& cur : rel) {
            rel2.insert(cur[0], cur[1], 1);
        }

        RamRelationStats full = RamRelationStats::extractFrom(rel2);

        RamRelationStats s10 = RamRelationStats::extractFrom(rel2, 10);
        RamRelationStats s100 = RamRelationStats::extractFrom(rel2, 100);
        RamRelationStats s1000 = RamRelationStats::extractFrom(rel2, 1000);
        RamRelationStats s10000 = RamRelationStats::extractFrom(rel2, 10000);

        for(int i =0 ; i < 3; i++) {
            std::cout << "Card " << i << ":\n";
            std::cout << "\t" <<    s10.getEstimatedCardinality(i) << " - " <<  (s10.getEstimatedCardinality(i) - full.getEstimatedCardinality(i)) << "\n";
            std::cout << "\t" <<   s100.getEstimatedCardinality(i) << " - " <<  (s100.getEstimatedCardinality(i) - full.getEstimatedCardinality(i)) << "\n";
            std::cout << "\t" <<  s1000.getEstimatedCardinality(i) << " - " <<  (s1000.getEstimatedCardinality(i) - full.getEstimatedCardinality(i)) << "\n";
            std::cout << "\t" << s10000.getEstimatedCardinality(i) << " - " <<  (s10000.getEstimatedCardinality(i) - full.getEstimatedCardinality(i)) << "\n";
            std::cout << "\t" <<   full.getEstimatedCardinality(i) << " - " <<  (full.getEstimatedCardinality(i) - full.getEstimatedCardinality(i)) << "\n";

            std::cout << "\n";
        }

    }


} // end namespace test
