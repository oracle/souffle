/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file btree_set_test.h
 *
 * A test case testing the B-trees utilization as sets.
 *
 ***********************************************************************/

#include "BTree.h"
#include "test.h"

#include <algorithm>
#include <chrono>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <set>
#include <tuple>
#include <unordered_set>
#include <vector>

namespace std {


template <typename A, typename B>
struct hash<tuple<A, B>> {
    std::size_t operator()(const tuple<A, B>& t) const {
        auto a = std::hash<A>()(get<0>(t));
        auto b = std::hash<B>()(get<1>(t));
        // from http://www.boost.org/doc/libs/1_35_0/doc/html/boost/hash_combine_id241013.html
        return a ^ (b + 0x9e3779b9 + (a << 6) + (a >> 2));
    }
};

template <typename A, typename B>
std::ostream& operator<<(std::ostream& out, const tuple<A, B>& t) {
    return out << "[" << get<0>(t) << "," << get<1>(t) << "]";
}
}

namespace souffle {

namespace test {


using namespace std;
using namespace std::chrono;

TEST(BTreeSet, Basic) {

    const bool DEBUG = false;

    typedef btree_set<int, detail::comparator<int>, std::allocator<int>, 16> test_set;

    test_set t;

    EXPECT_EQ(3, test_set::max_keys_per_node);

    // check initial conditions
    EXPECT_EQ(0u, t.size());
    EXPECT_FALSE(t.contains(10));
    EXPECT_FALSE(t.contains(12));
    EXPECT_FALSE(t.contains(14));
    EXPECT_EQ(0, t.getDepth());
    EXPECT_EQ(0, t.getNumNodes());

    if (DEBUG) t.printTree();

    // add an element

    t.insert(12);
    if (DEBUG) {
        t.printTree();
        std::cout << "\n";
    }

    EXPECT_EQ(1u, t.size());
    EXPECT_FALSE(t.contains(10));
    EXPECT_TRUE(t.contains(12));
    EXPECT_FALSE(t.contains(14));
    EXPECT_EQ(1, t.getDepth());
    EXPECT_EQ(1, t.getNumNodes());

    // add a larger element

    t.insert(14);
    if (DEBUG) {
        t.printTree();
        std::cout << "\n";
    }
    EXPECT_EQ(2u, t.size());
    EXPECT_FALSE(t.contains(10));
    EXPECT_TRUE(t.contains(12));
    EXPECT_TRUE(t.contains(14));
    EXPECT_EQ(1, t.getDepth());
    EXPECT_EQ(1, t.getNumNodes());

    // add a smaller element

    t.insert(10);
    if (DEBUG) {
        t.printTree();
        std::cout << "\n";
    }
    EXPECT_EQ(3u, t.size());
    EXPECT_TRUE(t.contains(10));
    EXPECT_TRUE(t.contains(12));
    EXPECT_TRUE(t.contains(14));
    EXPECT_EQ(1, t.getDepth());
    EXPECT_EQ(1, t.getNumNodes());

    // cause a split
    t.insert(11);
    if (DEBUG) {
        t.printTree();
        std::cout << "\n";
    }
    EXPECT_EQ(4u, t.size());
    EXPECT_TRUE(t.contains(10));
    EXPECT_TRUE(t.contains(11));
    EXPECT_TRUE(t.contains(12));
    EXPECT_TRUE(t.contains(14));

    if (DEBUG) {
        t.printTree();
        std::cout << "\n";
    }

    EXPECT_EQ(4u, t.size());
    t.insert(12);
    EXPECT_EQ(4u, t.size());
    t.insert(12);
    EXPECT_EQ(4u, t.size());

    t.insert(10);
    EXPECT_EQ(4u, t.size());

    if (DEBUG) {
        t.printTree();
        std::cout << "\n";
    }

    t.insert(15);
    EXPECT_EQ(5u, t.size());
    EXPECT_EQ(2, t.getDepth());
    EXPECT_EQ(3, t.getNumNodes());
    if (DEBUG) {
        t.printTree();
        std::cout << "\n";
    }

    t.insert(16);
    EXPECT_EQ(6u, t.size());
    if (DEBUG) {
        t.printTree();
        std::cout << "\n";
    }
}

TEST(BTreeSet, Duplicates) {
    typedef btree_set<int, detail::comparator<int>, std::allocator<int>, 16> test_set;

    test_set t;

    for (int i = 0; i < 10; i++) {
        t.insert(0);
    }

    EXPECT_EQ(1, t.size());

    EXPECT_EQ(0, *t.begin());

    //        t.printTree();
}

TEST(BTreeSet, Incremental) {
    typedef btree_set<int, detail::comparator<int>, std::allocator<int>, 16> test_set;

    test_set t;

    int N = 1000;

    for (int i = 0; i < N; i++) {
        //        std::cout << "\nBefore:\n";
        //        t.printTree();
        //        std::cout << "\n";
        t.insert(i);
        //        std::cout << "\nAfter:\n";
        //        std::cout << "\n";
        //        t.printTree();
        //        std::cout << "\n";
        //        std::cout << "\n\n";

        for (int j = 0; j < N; j++) {
            EXPECT_EQ(j <= i, t.contains(j)) << "i=" << i << ", j=" << j;
        }
    }

    t.printStats();
}

TEST(BTreeSet, Decremental) {
    typedef btree_set<int, detail::comparator<int>, std::allocator<int>, 16> test_set;

    test_set t;

    int N = 1000;

    for (int i = N; i >= 0; i--) {
        //            std::cout << "\nBefore:\n";
        //            t.printTree();
        //            std::cout << "\n";
        t.insert(i);
        //            std::cout << "\nAfter:\n";
        //            t.printTree();
        //            std::cout << "\n";
        //            std::cout << "\n\n";

        for (int j = 0; j < N; j++) {
            EXPECT_EQ(j >= i, t.contains(j)) << "i=" << i << ", j=" << j;
        }
    }
}

TEST(BTreeSet, Shuffled) {
    typedef btree_set<int, detail::comparator<int>, std::allocator<int>, 16> test_set;

    test_set t;

    int N = 10000;

    vector<int> data;
    for (int i = 0; i < N; i++) {
        data.push_back(i);
    }
    random_shuffle(data.begin(), data.end());

    for (int i = 0; i < N; i++) {
        t.insert(data[i]);
    }

    //        t.printTree();

    for (int i = 0; i < N; i++) {
        EXPECT_TRUE(t.contains(i)) << "i=" << i;
    }
}

TEST(BTreeSet, Copy) {
    typedef btree_set<int> test_set;

    test_set t;

    int N = 100000;

    vector<int> data;
    for (int i = 0; i < N; i++) {
        data.push_back(i);
    }
    random_shuffle(data.begin(), data.end());

    for (int i = 0; i < N; i++) {
        t.insert(data[i]);
    }

    EXPECT_EQ((size_t)N, t.size());

    for (int i = 0; i < N; i++) {
        EXPECT_TRUE(t.find(i) != t.end()) << "i=" << i;
    }

    test_set t2;
    EXPECT_EQ((size_t)N, t.size());
    EXPECT_EQ(0, t2.size());

    t2 = t;

    EXPECT_EQ((size_t)N, t.size());
    EXPECT_EQ((size_t)N, t2.size());

    for (int i = 0; i < N; i++) {
        EXPECT_TRUE(t.find(i) != t.end()) << "i=" << i;
    }
    for (int i = 0; i < N; i++) {
        EXPECT_TRUE(t2.find(i) != t2.end()) << "i=" << i;
    }

    EXPECT_FALSE(t.find(N + 1) != t.end());
    EXPECT_FALSE(t2.find(N + 1) != t2.end());
    t2.insert(N + 1);
    EXPECT_FALSE(t.find(N + 1) != t.end());
    EXPECT_TRUE(t2.find(N + 1) != t2.end());

    for (int i = 0; i < N; i++) {
        EXPECT_NE(&*t.find(i), &*t2.find(i)) << "i=" << i;
    }
}

TEST(BTreeSet, Merge) {
    typedef btree_set<int> test_set;

    test_set a;
    test_set b;

    a.insert(1);
    a.insert(2);
    a.insert(3);
    a.insert(4);

    b.insert(2);
    b.insert(4);
    b.insert(6);

    EXPECT_NE(a, b);

    test_set c = a;
    test_set d = b;

    EXPECT_NE(c, d);

    c.insertAll(b);
    d.insertAll(a);

    EXPECT_EQ(5, c.size());
    EXPECT_EQ(c, d);

    c.insertAll(a);
    EXPECT_EQ(c, d);
}

TEST(BTreeSet, IteratorEmpty) {
    typedef btree_set<int, detail::comparator<int>, std::allocator<int>, 16> test_set;
    test_set t;

    EXPECT_EQ(t.begin(), t.end());
}

TEST(BTreeSet, IteratorBasic) {
    typedef btree_set<int, detail::comparator<int>, std::allocator<int>, 16> test_set;
    test_set t;

    int N = 10;

    for (int i = 0; i <= N; i++) {
        t.insert(i);
    }

    //        t.printTree();

    auto it = t.begin();
    auto e = t.end();

    EXPECT_NE(it, e);

    int last = -1;
    for (int i : t) {
        EXPECT_EQ(last + 1, i);
        last = i;
    }
    EXPECT_EQ(last, N);
}

TEST(BTreeSet, IteratorStress) {
    typedef btree_set<int, detail::comparator<int>, std::allocator<int>, 16> test_set;

    test_set t;

    int N = 1000;

    vector<int> data;
    for (int i = 0; i < N; i++) {
        data.push_back(i);
    }
    random_shuffle(data.begin(), data.end());

    int max = -1;
    for (int i = 0; i < N; i++) {
        EXPECT_EQ((size_t)i, t.size());

        int last = -1;
        for (int i : t) {
            EXPECT_LT(last, i);
            last = i;
        }
        EXPECT_EQ(last, max);

        t.insert(data[i]);
        max = (data[i] > max) ? data[i] : max;
    }
}

TEST(BTreeSet, BoundaryTest) {
    typedef btree_set<int, detail::comparator<int>, std::allocator<int>, 16> test_set;

    test_set t;

    for (int i = 0; i < 10; i++) {
        t.insert(i);
    }

    //    t.printTree();

    auto a = t.lower_bound(5);
    EXPECT_EQ(5, *a);

    auto b = t.upper_bound(5);
    EXPECT_EQ(6, *b);

    // add duplicates

    t.insert(5);
    t.insert(5);
    t.insert(5);

    //    t.printTree(); std::cout << "\n";

    // test again ..
    a = t.lower_bound(5);
    EXPECT_EQ(5, *a);

    b = t.upper_bound(5);
    EXPECT_EQ(6, *b);

    // check the distance
    EXPECT_EQ(++a, b);
}

TEST(BTreeSet, BoundaryEmpty) {
    typedef btree_set<int, detail::comparator<int>, std::allocator<int>, 16> test_set;

    test_set t;

    EXPECT_EQ(t.end(), t.lower_bound(5));
    EXPECT_EQ(t.end(), t.upper_bound(5));

    t.insert(4);

    EXPECT_EQ(t.lower_bound(3), t.upper_bound(3));
    EXPECT_EQ(t.lower_bound(5), t.upper_bound(5));

    t.insert(6);
    EXPECT_EQ(t.lower_bound(3), t.upper_bound(3));
    EXPECT_EQ(t.lower_bound(5), t.upper_bound(5));

    t.insert(5);

    EXPECT_EQ(t.lower_bound(3), t.upper_bound(3));
    EXPECT_NE(t.lower_bound(5), t.upper_bound(5));
}

TEST(BTreeSet, Load) {
    typedef btree_set<int, detail::comparator<int>, std::allocator<int>, 16> test_set;

    for (int N = 0; N < 100; N++) {
        // generate some ordered data
        vector<int> data;

        for (int i = 0; i < N; i++) {
            data.push_back(i);
        }

        auto t = test_set::load(data.begin(), data.end());
        //        t.printTree();

        EXPECT_EQ(data.size(), t.size());
        EXPECT_TRUE(t.check());

        int last = -1;
        for (int c : t) {
            EXPECT_EQ(last + 1, c);
            last = c;
        }
        EXPECT_EQ(last, N - 1);
    }
}

TEST(BTreeSet, Clear) {
    typedef btree_set<int, detail::comparator<int>, std::allocator<int>, 16> test_set;

    test_set t;

    EXPECT_TRUE(t.empty());

    t.insert(5);

    EXPECT_FALSE(t.empty());
    t.clear();
    EXPECT_TRUE(t.empty());

    t.clear();
    EXPECT_TRUE(t.empty());
}

TEST(BTreeSet, ChunkSplit) {
    typedef btree_set<int, detail::comparator<int>, std::allocator<int>, 16> test_set;

    test_set t;

    // TODO: test for empty

    for (int i = 0; i < 100; i++) {
        t.insert(i);
    }

    // split chunks
    auto chunks = t.getChunks(20);

    //        EXPECT_EQ(20, chunks.size());

    for (const auto& cur : chunks) {
        for (auto i = cur.begin(); i != cur.end(); ++i) {
            std::cout << *i << ", ";
        }
        std::cout << "\n";
    }

    int last = -1;
    for (const auto& cur : chunks) {
        for (auto i = cur.begin(); i != cur.end(); ++i) {
            EXPECT_EQ(last + 1, *i);
            last = *i;
        }
    }
}

TEST(BTreeSet, ChunkSplitStress) {
    typedef btree_set<int, detail::comparator<int>, std::allocator<int>, 16> test_set;

    for (int i = 0; i < 1000; i++) {
        // generate random sequence
        vector<int> data;
        for (int j = 0; j < i; j++) {
            data.push_back(j);
        }
        random_shuffle(data.begin(), data.end());

        // fill tree
        test_set t;

        for (int x : data) t.insert(x);

        for (int j = 1; j < 100; j++) {
            auto chunks = t.getChunks(j);
            //                EXPECT_LT(chunks.size(), j * 2);

            if (chunks.empty()) continue;

            // check covered range
            EXPECT_EQ(0, *chunks.front().begin());

            int last = -1;
            for (const auto& cur : chunks) {
                for (auto it = cur.begin(); it != cur.end(); ++it) {
                    EXPECT_EQ(last + 1, *it);
                    last = *it;
                }
            }

            EXPECT_EQ(i - 1, last);
        }
    }
}

typedef tuple<int, int> Entry;

vector<Entry> getData(unsigned numEntries) {
    vector<Entry> res(numEntries);
    int k = 0;
    for (unsigned i = 0; i < numEntries; i++) {
        res[k++] = Entry(i / 100, i % 100);
    }
    random_shuffle(res.begin(), res.end());
    return res;
}

typedef high_resolution_clock::time_point time_point;

time_point now() {
    return high_resolution_clock::now();
}

long duration(const time_point& start, const time_point& end) {
    return std::chrono::duration_cast<chrono::milliseconds>(end - start).count();
}

template <typename Op>
long time(const string& name, const Op& operation) {
    std::cout << "\t" << setw(30) << setiosflags(ios::left) << name << resetiosflags(ios::left) << " ... "
              << std::flush;
    auto a = now();
    operation();
    auto b = now();
    auto time = duration(a, b);
    std::cout << " done [" << setw(5) << time << "ms]\n";
    return time;
}

template <typename C>
struct reserver {
    void operator()(C&, unsigned) const {
        // default: no action
    }
};

template <typename A, typename B, typename C, typename D>
struct reserver<std::unordered_set<A, B, C, D>> {
    void operator()(std::unordered_set<A, B, C, D>& set, unsigned size) const {
        set.reserve(size);
    }
};

#define checkPerformance(set_type, name, in, out)                                      \
    {                                                                                  \
        std::cout << "Testing: " << name << " ..\n";                                   \
        set_type set;                                                                  \
        time("filling set", [&]() {                                                    \
            reserver<set_type>()(set, in.size());                                      \
            for (const auto& cur : in) {                                               \
                set.insert(cur);                                                       \
            }                                                                          \
        });                                                                            \
        EXPECT_EQ(in.size(), set.size());                                              \
        int counter = 0;                                                               \
        time("full scan", [&]() {                                                      \
            for (auto it = set.begin(); it != set.end(); ++it) {                       \
                counter++;                                                             \
            }                                                                          \
        });                                                                            \
        EXPECT_EQ(in.size(), (size_t)counter);                                         \
        bool allPresent = true;                                                        \
        time("membership in", [&]() {                                                  \
            for (const auto& cur : in) {                                               \
                allPresent = (set.find(cur) != set.end()) && allPresent;               \
            }                                                                          \
        });                                                                            \
        EXPECT_TRUE(allPresent);                                                       \
        bool allMissing = true;                                                        \
        time("membership out", [&]() {                                                 \
            for (const auto& cur : out) {                                              \
                allMissing = (set.find(cur) == set.end()) && allMissing;               \
            }                                                                          \
        });                                                                            \
        EXPECT_TRUE(allMissing);                                                       \
        bool allFound = true;                                                          \
        time("lower_boundaries", [&]() {                                               \
            for (const auto& cur : in) {                                               \
                allFound = (set.lower_bound(cur) == set.find(cur)) && allFound;        \
            }                                                                          \
        });                                                                            \
        EXPECT_TRUE(allFound);                                                         \
        allFound = true;                                                               \
        time("upper_boundaries", [&]() {                                               \
            for (const auto& cur : in) {                                               \
                allFound = (set.upper_bound(cur) == (++set.find(cur))) && allFound;    \
            }                                                                          \
        });                                                                            \
        EXPECT_TRUE(allFound);                                                         \
        allFound = true;                                                               \
        time("boundaries on missing elements", [&]() {                                 \
            for (const auto& cur : out) {                                              \
                allFound = (set.lower_bound(cur) == set.upper_bound(cur)) && allFound; \
            }                                                                          \
        });                                                                            \
        EXPECT_TRUE(allFound);                                                         \
        set_type a(in.begin(), in.end());                                              \
        set_type b(out.begin(), out.end());                                            \
        time("merge two sets", [&]() { a.insert(b.begin(), b.end()); });               \
        std::cout << "\tDone!\n\n";                                                    \
    }

TEST(Performance, Basic) {
    //        int N = 1<<22;
    int N = 1 << 18;

    // get list of tuples to be inserted
    std::cout << "Generating Test-Data ...\n";
    vector<Entry> in;
    vector<Entry> out;
    time("generating data", [&]() {
        auto data = getData(2 * N);
        for (std::size_t i = 0; i < data.size(); i += 2) {
            in.push_back(data[i]);
            out.push_back(data[i + 1]);
        }
    });

    typedef std::set<Entry> t1;
    checkPerformance(t1, " -- warm up -- ", in, out);

    typedef btree_set<Entry, detail::comparator<Entry>, std::allocator<Entry>, 256, detail::linear_search> t2;
    checkPerformance(t2, "souffle btree_set - 256 - linear", in, out);

    typedef btree_set<Entry, detail::comparator<Entry>, std::allocator<Entry>, 256, detail::binary_search> t3;
    checkPerformance(t3, "souffle btree_set - 256 - binary", in, out);
}

TEST(Performance, Load) {
    //        int N = 1<<24;
    int N = 1 << 20;

    vector<int> data;
    for (int i = 0; i < N; i++) {
        data.push_back(i);
    }

    // take time for conventional load
    time("conventional load", [&]() { btree_set<int> t(data.begin(), data.end()); });

    // take time for structured load
    time("bulk-load", [&]() { auto t = btree_set<int>::load(data.begin(), data.end()); });
}

TEST(BTreeSet, Parallel) {
    //        const int N = 600000000;
    //        const int N = 100000;
    //        const int N = 10000;
    const int N = 1000;
    //        const int N = 300;
    //        const int N = 100;

    // get a unordered list of test data
    typedef int entry_t;
    std::vector<entry_t> list;
    btree_set<entry_t> filter;

    for (int i = 0; i < N; i++) {
        list.push_back(i);
    }

    // the number of times duplicates show up in the input set
    for (int dup = 1; dup < 4; dup++) {
        // now duplicate this list
        std::vector<entry_t> full;
        for (int i = 0; i < dup; i++) {
            for (const auto& cur : list) {
                full.push_back(cur);
            }
        }

        // shuffle data
        std::random_shuffle(full.begin(), full.end());

        // now insert all those values into a new set - in parallel
        btree_set<entry_t> res;
#pragma omp parallel for  //  schedule(static,1)
        for (std::vector<entry_t>::iterator it = full.begin(); it < full.end(); ++it) {
            res.insert(*it);
        }

        EXPECT_TRUE(res.check());

        // check resulting values
        EXPECT_EQ(N, res.size());

        std::set<entry_t> should(full.begin(), full.end());
        std::set<entry_t> is(res.begin(), res.end());

        for (const auto& cur : should) {
            EXPECT_TRUE(res.contains(cur)) << "Missing: " << cur << "\n";
        }

        for (const auto& cur : res) {
            EXPECT_TRUE(should.find(cur) != should.end()) << "Additional: " << cur << "\n"
                                                          << "Contained: " << res.contains(cur) << "\n";
        }

        std::vector<entry_t> extra;
        for (const auto& cur : is) {
            if (should.find(cur) == should.end()) extra.push_back(cur);
        }
        EXPECT_TRUE(extra.empty()) << "Extra elments: " << extra << "\n";

        std::vector<entry_t> missing;
        for (const auto& cur : should) {
            if (is.find(cur) == is.end()) missing.push_back(cur);
        }
        EXPECT_TRUE(missing.empty()) << "Missing elments: " << missing << "\n";
        //                << "All Elements: " << should << "\n";

        EXPECT_EQ(N, should.size());
        EXPECT_EQ(N, is.size());
        EXPECT_EQ(should, is);
    }
}

#ifdef _OPENMP

TEST(BTreeSet, ParallelScaling) {
    typedef btree_set<int> test_set;
    typedef test_set::operation_hints op_context_type;

    //        const int N = 60000000;     // real benchmark
    //        const int N = 1000000;     // to not run to long for unit testing
    //        const int N = 100000;     // to not run to long for unit testing
    const int N = 1000;  // to not run to long for unit testing

    // create some random data
    vector<int> data;
    for (int i = 0; i < N; i++) {
        data.push_back(i);
    }
    vector<int> data2 = data;
    random_shuffle(data.begin(), data.end());
    random_shuffle(data2.begin(), data2.end());

    for (int i = 1; i <= 8; i++) {
        test_set t;

        omp_set_num_threads(i);

        double start = omp_get_wtime();

#pragma omp parallel
        {
            op_context_type ctxt;

#pragma omp for
            for (int i = 0; i < N; i++) {
                t.insert(data[i], ctxt);
                t.insert(data2[i], ctxt);
            }
        }

        double end = omp_get_wtime();

        std::cout << "Number of threads: " << i << "[" << (end - start) << "ms]\n";

        //          t.printTree();

        EXPECT_EQ(N, t.size());
        int count = 0;
        int last = -1;
        for (int i : t) {
            EXPECT_EQ(last + 1, i);
            last = i;
            count++;
        }
        EXPECT_EQ(N - 1, last);

        EXPECT_EQ(N, count);
    }
}

#endif
}
}  // end namespace souffle
