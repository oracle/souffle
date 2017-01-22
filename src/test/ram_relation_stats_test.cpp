/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
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
#include <memory>

#include "RamRelationStats.h"
#include "IOSystem.h"
#include "ReadStream.h"

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

            std::string options("name=");
            options += (BUILDDIR "../tests/evaluation/hmmer/facts/DirectFlow.facts");
            options += ",delimiter=\t,";
	    options += "IO=file";
            // if file not found => be done
            if (!in.is_open()) return;

            std::unique_ptr<ReadStream> reader =
                    IOSystem::getInstance().getReader(mask, symTable, options);

            while (auto next = reader->readNextTuple()) {
                rel.insert(next.get());
            }
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
