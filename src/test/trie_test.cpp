/*
 * Souffle - A Datalog Compiler
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - <souffle root>/licenses/SOUFFLE-UPL.txt
 */

/************************************************************************
 *
 * @file trie_test.h
 *
 * A test case testing the trie implementation.
 *
 ***********************************************************************/

#include "test.h"

#include "string.h"
#include "Trie.h"

using namespace souffle;

TEST(SparseArray, Basic) {
    SparseArray<int> map;

    EXPECT_EQ(0, map[10]);
    EXPECT_EQ(0, map[12]);
    EXPECT_EQ(0, map[14]);
    EXPECT_EQ(0, map[120]);

    EXPECT_EQ(0, map[10]);
    EXPECT_EQ(0, map[12]);
    EXPECT_EQ(0, map[14]);
    EXPECT_EQ(0, map[120]);

    map.update(12, 1);

    EXPECT_EQ(0, map[10]);
    EXPECT_EQ(1, map[12]);
    EXPECT_EQ(0, map[14]);
    EXPECT_EQ(0, map[120]);

    map.update(14, 8);

    EXPECT_EQ(0, map[10]);
    EXPECT_EQ(1, map[12]);
    EXPECT_EQ(8, map[14]);
    EXPECT_EQ(0, map[120]);

    map.update(120, 4);

    EXPECT_EQ(0, map[10]);
    EXPECT_EQ(1, map[12]);
    EXPECT_EQ(8, map[14]);
    EXPECT_EQ(4, map[120]);
}

TEST(SparseArray, Limits) {

    SparseArray<int> map;

    map.update(std::numeric_limits<typename SparseArray<int>::index_type>::min(), 10);
    map.update(std::numeric_limits<typename SparseArray<int>::index_type>::max(), 20);

    map.dump();

    std::vector<std::pair<uint32_t,int>> present;
    for(const auto& cur : map) {
        present.push_back(cur);
    }

    EXPECT_EQ("[(0,10),(4294967295,20)]", toString(present));

}

TEST(SparseArray, Iterator) {
    SparseArray<int> map;

    std::set<std::pair<int,int>> should;
    should.insert(std::make_pair(14,4));
    should.insert(std::make_pair(0,1));
    should.insert(std::make_pair(4,2));
    should.insert(std::make_pair(38,5));
    should.insert(std::make_pair(12,3));
    should.insert(std::make_pair(120,6));

    for(const auto& cur : should) {
        map.update(cur.first, cur.second);
    }

    std::set<std::pair<int,int>> is;
    for(const auto& cur : map) {
        is.insert(cur);
    }

    EXPECT_EQ(should, is);
}

TEST(SparseArray, IteratorStress) {
    const static int N = 10000;

    SparseArray<int> map;

    std::vector<int> pos;
    while(pos.size() < N) {
        int n = random() % (N*10);
        if (!contains(pos, n)) {
            pos.push_back(n);
        }
    }

    std::set<std::pair<int,int>> should;
    for(int i=0; i<N; i++) {
        should.insert(std::make_pair(pos[i],i+1));
    }

    for(const auto& cur : should) {
        map.update(cur.first, cur.second);
        ASSERT_TRUE(map[cur.first] == cur.second);
    }

    std::set<std::pair<int,int>> is;
    for(const auto& cur : map) {
        is.insert(cur);
    }

    EXPECT_EQ(should, is);
}


TEST(SparseArray, IteratorStress2) {
    const static int N = 1000;

    bool log = false;

    for(unsigned j=0; j<N; j++) {

        SparseArray<int> map;

        if (log) std::cout << "Creating " << j << " random numbers ..\n";
        std::vector<int> pos;
        while(pos.size() < j) {
            int n = random() % (N*10);
            if (!contains(pos, n)) {
                pos.push_back(n);
            }
        }

        if (log) std::cout << "Creating input list ..\n";
        std::set<std::pair<int,int>> should;
        for(unsigned i=0; i<j; i++) {
            should.insert(std::make_pair(pos[i],i+1));
        }

        if (log) std::cout << "Filling in input list ..\n";
        for(const auto& cur : should) {
            map.update(cur.first, cur.second);
            ASSERT_TRUE(map[cur.first] == cur.second);
        }

        if (log) std::cout << "Sort should list ..\n";

        if (log) std::cout << "Collect is list ..\n";
        std::set<std::pair<int,int>> is;
        unsigned i = 0;
        for(const auto& cur : map) {
            is.insert(cur);
            i++;
            ASSERT_LE(i,j);
        }

        if (log) std::cout << "Comparing lists ..\n";
        EXPECT_EQ(should, is);
        if (log) std::cout << "Done\n\n";
    }
}

TEST(SparseArray, Find) {

    SparseArray<int> map;

    EXPECT_EQ(map.end(), map.find(1));
    EXPECT_EQ(map.end(), map.find(12));
    EXPECT_EQ(map.end(), map.find(1400));

    map.update(1400,1);

    EXPECT_EQ(map.end(), map.find(1));
    EXPECT_EQ(map.end(), map.find(12));
    EXPECT_NE(map.end(), map.find(1400));

    EXPECT_EQ("(1400,1)", toString(*map.find(1400)));

    map.update(12,2);

    EXPECT_EQ(map.end(), map.find(1));
    EXPECT_NE(map.end(), map.find(12));
    EXPECT_NE(map.end(), map.find(1400));

    EXPECT_EQ("(12,2)", toString(*map.find(12)));
    EXPECT_EQ("(1400,1)", toString(*map.find(1400)));

    auto it = map.find(12);
    EXPECT_EQ("(12,2)", toString(*it));
    ++it;
    EXPECT_EQ("(1400,1)", toString(*it));
}

TEST(SparseArray, Find2) {

    SparseArray<int> a;

    EXPECT_EQ(a.end(), a.find(12));
    EXPECT_EQ(a.end(), a.find(14));
    EXPECT_EQ(a.end(), a.find(16));

    a.update(14,4);

    EXPECT_EQ(a.end(), a.find(12));
    EXPECT_NE(a.end(), a.find(14));
    EXPECT_EQ(a.end(), a.find(16));

    a.update(16,6);

    EXPECT_EQ(a.end(), a.find(12));
    EXPECT_NE(a.end(), a.find(14));
    EXPECT_NE(a.end(), a.find(16));
}

TEST(SparseArray, Copy) {

    SparseArray<int> m;

    m.update(12,1);
    m.update(14,2);
    m.update(16,3);

    auto a = m;

    EXPECT_EQ(1, m[12]);
    EXPECT_EQ(2, m[14]);
    EXPECT_EQ(3, m[16]);

    EXPECT_EQ(1, a[12]);
    EXPECT_EQ(2, a[14]);
    EXPECT_EQ(3, a[16]);

    m = a;

    EXPECT_EQ(1, m[12]);
    EXPECT_EQ(2, m[14]);
    EXPECT_EQ(3, m[16]);

    EXPECT_EQ(1, a[12]);
    EXPECT_EQ(2, a[14]);
    EXPECT_EQ(3, a[16]);
}


TEST(SparseArray, Merge) {
    // tests whether the first reference is properly updated while merging sets

    SparseArray<int> m1;
    SparseArray<int> m2;

    m1.update(500,2);
    m2.update(100,1);

    m1.addAll(m2);

    std::vector<std::pair<int,int>> data;
    for(auto it = m1.begin(); it!=m1.end(); ++it) {
        data.push_back(*it);
    }
    EXPECT_EQ("[(100,1),(500,2)]", toString(data));
}

TEST(SparseArray, LowerBound) {

    SparseArray<int> m;

    EXPECT_EQ(m.end(), m.lowerBound(0));
    EXPECT_EQ(m.end(), m.lowerBound(10));
    EXPECT_EQ(m.end(), m.lowerBound(12));
    EXPECT_EQ(m.end(), m.lowerBound(14));
    EXPECT_EQ(m.end(), m.lowerBound(400));
    EXPECT_EQ(m.end(), m.lowerBound(500));

    m.update(11,120);
    m.dump();
    EXPECT_EQ(m.begin(), m.lowerBound(0));
    EXPECT_EQ(m.find(11), m.lowerBound(10));
    EXPECT_EQ(m.end(), m.lowerBound(12));
    EXPECT_EQ(m.end(), m.lowerBound(14));
    EXPECT_EQ(m.end(), m.lowerBound(400));
    EXPECT_EQ(m.end(), m.lowerBound(500));

    m.update(12,140);
    m.dump();
    EXPECT_EQ(m.begin(), m.lowerBound(0));
    EXPECT_EQ(m.find(11), m.lowerBound(10));
    EXPECT_EQ(m.find(12), m.lowerBound(12));
    EXPECT_EQ(m.end(), m.lowerBound(14));
    EXPECT_EQ(m.end(), m.lowerBound(400));
    EXPECT_EQ(m.end(), m.lowerBound(500));

    m.update(300,150);
    m.dump();
    EXPECT_EQ(m.begin(), m.lowerBound(0));
    EXPECT_EQ(m.find(11), m.lowerBound(10));
    EXPECT_EQ(m.find(12), m.lowerBound(12));
    EXPECT_EQ(m.find(300), m.lowerBound(14));
    EXPECT_EQ(m.end(), m.lowerBound(400));
    EXPECT_EQ(m.end(), m.lowerBound(500));

    m.update(450,160);
    m.dump();
    EXPECT_EQ(m.begin(), m.lowerBound(0));
    EXPECT_EQ(m.find(11), m.lowerBound(10));
    EXPECT_EQ(m.find(12), m.lowerBound(12));
    EXPECT_EQ(m.find(300), m.lowerBound(14));
    std::cout << "\n";
    EXPECT_EQ(m.find(450), m.lowerBound(400));
    EXPECT_EQ(m.end(), m.lowerBound(500));
}

TEST(SparseArray, MemoryUsage) {

    if (sizeof(void*) > 4) { 
        SparseArray<int> a;

        // an empty one should be small
        EXPECT_TRUE(a.empty());
        // EXPECT_EQ(56, a.getMemoryUsage());
        EXPECT_EQ(40, a.getMemoryUsage());

        // a single element should have the same size as an empty one
        a.update(12, 15);
        EXPECT_FALSE(a.empty());
        // EXPECT_EQ(56, a.getMemoryUsage());
        EXPECT_EQ(560, a.getMemoryUsage());

        // more than one => there are nodes
        a.update(14, 18);
        EXPECT_FALSE(a.empty());

        //EXPECT_EQ(576, a.getMemoryUsage());
        EXPECT_EQ(560, a.getMemoryUsage());
    }  else {
        SparseArray<int> a;

        // an empty one should be small
        EXPECT_TRUE(a.empty());
        EXPECT_EQ(28, a.getMemoryUsage());

        // a single element should have the same size as an empty one
        a.update(12, 15);
        EXPECT_FALSE(a.empty());
        EXPECT_EQ(288, a.getMemoryUsage());

        // more than one => there are nodes
        a.update(14, 18);
        EXPECT_FALSE(a.empty());
        EXPECT_EQ(288, a.getMemoryUsage());
    }
}


TEST(SparseBitMap, Basic) {

    SparseBitMap<> map;

    EXPECT_EQ(sizeof(std::bitset<sizeof(void*)*8>), sizeof(void*));

    EXPECT_FALSE(map[12]);
    EXPECT_FALSE(map[120]);
    EXPECT_FALSE(map[84]);

    map.set(12);

    EXPECT_TRUE(map[12]);
    EXPECT_FALSE(map[120]);
    EXPECT_FALSE(map[84]);

    map.set(120);

    EXPECT_TRUE(map[12]);
    EXPECT_TRUE(map[120]);
    EXPECT_FALSE(map[84]);

    map.set(84);

    EXPECT_TRUE(map[12]);
    EXPECT_TRUE(map[120]);
    EXPECT_TRUE(map[84]);
}

TEST(SparseBitMap,Stress) {

    const static int N = 10000;

    SparseBitMap<> map;

    std::vector<int> should;
    while(should.size() < N) {
        int n = random() % (N*10);
        if (!contains(should, n)) {
            should.push_back(n);
        }
    }

    for(const auto& cur : should) {
        map.set(cur);
        ASSERT_TRUE(map[cur]);
    }

    // check all the entries
    for(int i=0; i<N*10; i++) {
        EXPECT_EQ(map[i], contains(should, i));
    }

}

TEST(SparseBitMap, Iterator) {

    SparseBitMap<> map;

    std::set<int> vals;
    for(const auto& cur : map) {
        vals.insert(cur);
    }

    EXPECT_EQ("{}", toString(vals));

    map.set(12);

    vals.clear();
    for(const auto& cur : map) {
        vals.insert(cur);
    }

    EXPECT_EQ("{12}", toString(vals));

    map.set(12);
    map.set(120);

    vals.clear();
    for(const auto& cur : map) {
        vals.insert(cur);
    }

    EXPECT_EQ("{12,120}", toString(vals));


    map.set(1234);

    vals.clear();
    for(const auto& cur : map) {
        vals.insert(cur);
    }

    EXPECT_EQ("{12,120,1234}", toString(vals));
}

TEST(SparseBitMap, IteratorStress2) {
    const static int N = 1000;

    bool log = false;

    for(unsigned j=0; j<N; j++) {

        SparseBitMap<> map;

        if (log) std::cout << "Creating " << j << " random numbers ..\n";
        std::set<int> should;
        while(should.size() < j) {
            int n = random() % (N*10);
            if (!contains(should, n)) {
                should.insert(n);
            }
        }

        if (log) std::cout << "Filling in input list ..\n";
        for(const auto& cur : should) {
            map.set(cur);
            ASSERT_TRUE(map[cur]);
        }

        if (log) std::cout << "Collect is list ..\n";
        std::set<int> is;
        unsigned i = 0;
        for(const auto& cur : map) {
            is.insert(cur);
            i++;

            if (i > j) {
                map.dump();
                std::cout << "Should: " << should << "\n";
                std::cout << "    Is: " << is << "\n";
            }

            ASSERT_LE(i,j);
        }

        if (log) std::cout << "Comparing lists ..\n";
        EXPECT_EQ(should, is);
        if (log) std::cout << "Done\n\n";

    }
}

TEST(SparseBitMap, Find) {

    SparseBitMap<> map;

    EXPECT_EQ(map.end(), map.find(1));
    EXPECT_EQ(map.end(), map.find(12));
    EXPECT_EQ(map.end(), map.find(1400));

    map.set(1400);

    EXPECT_EQ(map.end(), map.find(1));
    EXPECT_EQ(map.end(), map.find(12));
    EXPECT_NE(map.end(), map.find(1400));

    EXPECT_EQ("1400", toString(*map.find(1400)));

    map.set(12);

    EXPECT_EQ(map.end(), map.find(1));
    EXPECT_NE(map.end(), map.find(12));
    EXPECT_NE(map.end(), map.find(1400));

    EXPECT_EQ("12", toString(*map.find(12)));
    EXPECT_EQ("1400", toString(*map.find(1400)));

    auto it = map.find(12);
    EXPECT_EQ("12", toString(*it));
    ++it;
    EXPECT_EQ("1400", toString(*it));
}

TEST(SparseBitMap, Size) {
    SparseBitMap<> map;
    EXPECT_EQ(0, map.size());
    map.set(3);
    EXPECT_EQ(1, map.size());
    map.set(5);
    EXPECT_EQ(2, map.size());
    map.set(3);
    EXPECT_EQ(2, map.size());
    map.set(1000);
    EXPECT_EQ(3, map.size());
}

TEST(SparseBitMap, CopyAndMerge) {
    SparseBitMap<> mapA;
    SparseBitMap<> mapB;
    SparseBitMap<> mapC;

    mapA.set(3);
    mapA.set(4);
    mapA.set(5);

    mapB.set(10000000);
    mapB.set(10000001);
    mapB.set(10000002);

    mapC.set(3);
    mapC.set(7);
    mapC.set(10000000);
    mapC.set(10000007);


    auto m = mapA;
    EXPECT_EQ(3, m.size());

    for(const auto& cur : m) {
        EXPECT_TRUE(
                mapA.test(cur)
        );
    }

    m.addAll(mapA);
    EXPECT_EQ(3, m.size());

    for(const auto& cur : m) {
        EXPECT_TRUE(
                mapA.test(cur)
        );
    }

    m.addAll(mapB);
    EXPECT_EQ(6, m.size());

    for(const auto& cur : m) {
        EXPECT_TRUE(
                mapA.test(cur) || mapB.test(cur)
        );
    }

    m.addAll(mapC);
    EXPECT_EQ(8, m.size());

    for(const auto& cur : m) {
        EXPECT_TRUE(
                mapA.test(cur) || mapB.test(cur) || mapC.test(cur)
        );
    }

}


TEST(Trie, Basic) {

    Trie<1> set;

    EXPECT_TRUE(set.empty());
    EXPECT_FALSE(set.contains(1));
    EXPECT_FALSE(set.contains(2));
    EXPECT_FALSE(set.contains(3));

    set.insert(1);

    EXPECT_TRUE(set.contains(1));
    EXPECT_FALSE(set.contains(2));
    EXPECT_FALSE(set.contains(3));

    set.insert(2);

    EXPECT_TRUE(set.contains(1));
    EXPECT_TRUE(set.contains(2));
    EXPECT_FALSE(set.contains(3));

}


namespace {

    template<typename Iter>
    int card(const range<Iter>& r) {
        int res = 0;
        for(auto it = r.begin(); it!=r.end(); ++it) res++;
        return res;
    }

    template<typename Container>
    int card(const Container& c) {
        return card(make_range(c.begin(), c.end()));
    }

}

TEST(Trie, Iterator) {

    Trie<2> set;

    EXPECT_EQ(set.begin(), set.end());

    set.insert(1,2);

    EXPECT_NE(set.begin(), set.end());

    set.insert(4,3);
    set.insert(5,2);

    EXPECT_NE(set.begin(), set.end());

    EXPECT_EQ(3,card(set));
}

TEST(Trie, IteratorStress_0D) {

    Trie<0> set;

    EXPECT_TRUE(set.empty());

    int count = 0;
    for(auto it = set.begin(); it!=set.end(); ++it) {
        count++;
    }

    EXPECT_EQ(0, count);
    EXPECT_EQ(0,set.size());

    set.insert();

    EXPECT_FALSE(set.empty());
    EXPECT_EQ(1,set.size());

    set.insert();

    EXPECT_FALSE(set.empty());
    EXPECT_EQ(1,set.size());

    count = 0;
    for(auto it = set.begin(); it!=set.end(); ++it) {
        count++;
    }
    EXPECT_EQ(1, count);

}

namespace {

    RamDomain rand(RamDomain max) {
        return random() % max;
    }

}


TEST(Trie, IteratorStress_1D) {
    typedef typename ram::Tuple<RamDomain,1> tuple;

    const int N = 10000;

    Trie<1> set;

    std::set<tuple> data;
    while(data.size() < N) {
        tuple cur = tuple({{(RamDomain)(rand(N*10))}});
        if (data.insert(cur).second) {
            EXPECT_FALSE(set.contains(cur));
            set.insert(cur);
            EXPECT_TRUE(set.contains(cur));
        }
    }

    std::set<tuple> is;
    for(const auto& cur : set) {
        is.insert(cur);
    }

    EXPECT_EQ(N, set.size());
    EXPECT_EQ(data, is);
}

TEST(Trie, IteratorStress_2D) {
    typedef typename ram::Tuple<RamDomain,2> tuple;

    const int N = 10000;

    Trie<2> set;

    std::set<tuple> data;
    while(data.size() < N) {
        tuple cur;
        cur[0] = (RamDomain)(rand(N*10));
        cur[1] = (RamDomain)(rand(N*10));
        if (data.insert(cur).second) {
            EXPECT_FALSE(set.contains(cur));
            set.insert(cur);
            EXPECT_TRUE(set.contains(cur));
        }
    }

    std::set<tuple> is;
    for(const auto& cur : set) {
        is.insert(cur);
    }

    EXPECT_EQ(N, set.size());
    EXPECT_EQ(data, is);
}

TEST(Trie, IteratorStress_3D) {
    typedef typename ram::Tuple<RamDomain,3> tuple;

    const int N = 10000;

    Trie<3> set;

    std::set<tuple> data;
    while(data.size() < N) {
        tuple cur;
        cur[0] = (RamDomain)(rand(N*10));
        cur[1] = (RamDomain)(rand(N*10));
        cur[2] = (RamDomain)(rand(N*10));
        if (data.insert(cur).second) {
            EXPECT_FALSE(set.contains(cur));
            set.insert(cur);
            EXPECT_TRUE(set.contains(cur));
        }
    }

    std::set<tuple> is;
    for(const auto& cur : set) {
        is.insert(cur);
    }

    EXPECT_EQ(N, set.size());
    EXPECT_EQ(data, is);
}

TEST(Trie, IteratorStress_4D) {
    typedef typename ram::Tuple<RamDomain,4> tuple;

    const int N = 10000;

    Trie<4> set;

    std::set<tuple> data;
    while(data.size() < N) {
        tuple cur;
        cur[0] = (RamDomain)(rand(N*10));
        cur[1] = (RamDomain)(rand(N*10));
        cur[2] = (RamDomain)(rand(N*10));
        cur[3] = (RamDomain)(rand(N*10));
        if (data.insert(cur).second) {
            EXPECT_FALSE(set.contains(cur));
            set.insert(cur);
            EXPECT_TRUE(set.contains(cur));
        }
    }

    std::set<tuple> is;
    for(const auto& cur : set) {
        is.insert(cur);
    }

    EXPECT_EQ(N, set.size());
    EXPECT_EQ(data, is);
}


TEST(Trie, RangeQuery) {
    typedef typename ram::Tuple<RamDomain,3> tuple;

    Trie<3> set;

    for(int i=0; i<10; i++) {
        for(int j=0; j<10; j++) {
            for(int k=0; k<10; k++) {
                set.insert(i,j,k);
            }
        }
    }

    EXPECT_EQ(1000, set.size());

    // Range [*,*,*]
    EXPECT_EQ(1000,card(set.getBoundaries<0>(tuple({{3,4,5}}))));

    // Range [3,*,*]
    EXPECT_EQ( 100,card(set.getBoundaries<1>(tuple({{3,4,5}}))));

    // Range [3,4,*]
    EXPECT_EQ(  10,card(set.getBoundaries<2>(tuple({{3,4,5}}))));

    // Range [3,4,5]
    EXPECT_EQ(   1,card(set.getBoundaries<3>(tuple({{3,4,5}}))));

}

TEST(Trie, RangeQuery_0D) {
    typedef typename ram::Tuple<RamDomain,0> tuple;

    Trie<0> set;

    EXPECT_EQ(0, card(set.getBoundaries<0>(tuple())));

    set.insert();

    EXPECT_EQ(1, card(set.getBoundaries<0>(tuple())));

}

TEST(Trie, RangeQuery_1D) {
    typedef typename ram::Tuple<RamDomain,1> tuple;

    Trie<1> set;

    // empty set
    EXPECT_EQ(0, card(set.getBoundaries<0>(tuple({{3}}))));
    EXPECT_EQ(0, card(set.getBoundaries<1>(tuple({{3}}))));

    // add some elements
    for(int i=0; i<5; i++) {
        set.insert(i);
    }

    EXPECT_EQ(5, card(set.getBoundaries<0>(tuple({{3}}))));
    EXPECT_EQ(5, card(set.getBoundaries<0>(tuple({{7}}))));

    EXPECT_EQ(1, card(set.getBoundaries<1>(tuple({{3}}))));
    EXPECT_EQ(0, card(set.getBoundaries<1>(tuple({{7}}))));

}

TEST(Trie, RangeQuery_2D) {
    typedef typename ram::Tuple<RamDomain,2> tuple;

    Trie<2> set;

    // empty set
    EXPECT_EQ(0, card(set.getBoundaries<0>(tuple({{3,4}}))));
    EXPECT_EQ(0, card(set.getBoundaries<1>(tuple({{3,4}}))));
    EXPECT_EQ(0, card(set.getBoundaries<2>(tuple({{3,4}}))));

    // add some elements
    for(int i=0; i<5; i++) {
        for(int j=0; j<5; j++) {
            set.insert(i,j);
        }
    }

    EXPECT_EQ(25, card(set.getBoundaries<0>(tuple({{3,4}}))));
    EXPECT_EQ(25, card(set.getBoundaries<0>(tuple({{7,4}}))));
    EXPECT_EQ(25, card(set.getBoundaries<0>(tuple({{3,7}}))));

    EXPECT_EQ(5, card(set.getBoundaries<1>(tuple({{3,4}}))));
    EXPECT_EQ(0, card(set.getBoundaries<1>(tuple({{7,4}}))));
    EXPECT_EQ(5, card(set.getBoundaries<1>(tuple({{3,7}}))));

    EXPECT_EQ(1, card(set.getBoundaries<2>(tuple({{3,4}}))));
    EXPECT_EQ(0, card(set.getBoundaries<2>(tuple({{7,4}}))));
    EXPECT_EQ(0, card(set.getBoundaries<2>(tuple({{3,7}}))));

}

TEST(Trie, RangeQuery_3D) {
    typedef typename ram::Tuple<RamDomain,3> tuple;

    Trie<3> set;

    // empty set
    EXPECT_EQ(0, card(set.getBoundaries<0>(tuple({{3,4,2}}))));
    EXPECT_EQ(0, card(set.getBoundaries<1>(tuple({{3,4,2}}))));
    EXPECT_EQ(0, card(set.getBoundaries<2>(tuple({{3,4,2}}))));
    EXPECT_EQ(0, card(set.getBoundaries<3>(tuple({{3,4,2}}))));

    // add some elements
    for(int i=0; i<5; i++) {
        for(int j=0; j<5; j++) {
            for(int k=0; k<5; k++) {
                set.insert(i,j,k);
            }
        }
    }

    EXPECT_EQ(125, card(set.getBoundaries<0>(tuple({{3,4,2}}))));
    EXPECT_EQ(125, card(set.getBoundaries<0>(tuple({{7,4,2}}))));
    EXPECT_EQ(125, card(set.getBoundaries<0>(tuple({{3,7,2}}))));
    EXPECT_EQ(125, card(set.getBoundaries<0>(tuple({{3,7,8}}))));

    EXPECT_EQ(25, card(set.getBoundaries<1>(tuple({{3,4,2}}))));
    EXPECT_EQ( 0, card(set.getBoundaries<1>(tuple({{7,4,2}}))));
    EXPECT_EQ(25, card(set.getBoundaries<1>(tuple({{3,7,2}}))));
    EXPECT_EQ(25, card(set.getBoundaries<1>(tuple({{3,7,8}}))));

    EXPECT_EQ(5, card(set.getBoundaries<2>(tuple({{3,4,2}}))));
    EXPECT_EQ(0, card(set.getBoundaries<2>(tuple({{7,4,2}}))));
    EXPECT_EQ(0, card(set.getBoundaries<2>(tuple({{3,7,2}}))));
    EXPECT_EQ(0, card(set.getBoundaries<2>(tuple({{3,7,8}}))));
    EXPECT_EQ(5, card(set.getBoundaries<2>(tuple({{3,2,8}}))));

    EXPECT_EQ(1, card(set.getBoundaries<3>(tuple({{3,4,2}}))));
    EXPECT_EQ(0, card(set.getBoundaries<3>(tuple({{7,4,2}}))));
    EXPECT_EQ(0, card(set.getBoundaries<3>(tuple({{3,7,2}}))));
    EXPECT_EQ(0, card(set.getBoundaries<3>(tuple({{3,7,8}}))));

}



TEST(Trie, RangeQueryStress) {
    typedef typename ram::Tuple<RamDomain,3> tuple;

    Trie<3> set;

    for(int i=0; i<10; i++) {
        for(int j=0; j<10; j++) {
            for(int k=0; k<10; k++) {
                set.insert(i,j,k);
            }
        }
    }

    EXPECT_EQ(1000, set.size());

    // Range [*,*,*]
    EXPECT_EQ(1000, card(set.getBoundaries<0>(tuple({{3,4,5}}))));

    // Range [x,*,*]
    for(RamDomain x=0; x<10; x++) {
        EXPECT_EQ(100,card(set.getBoundaries<1>(tuple({{x,4,5}}))));
    }

    // Range [x,y,*]
    for(RamDomain x=0; x<10; x++) {
        for(RamDomain y=0; y<10; y++) {
            EXPECT_EQ(10,card(set.getBoundaries<2>(tuple({{x,y,5}}))));
        }
    }


    // Range [x,y,*]
    for(RamDomain x=0; x<10; x++) {
        for(RamDomain y=0; y<10; y++) {
            for(RamDomain z=0; z<10; z++) {
                EXPECT_EQ(1,card(set.getBoundaries<3>(tuple({{x,y,z}}))));
            }
        }
    }

}

TEST(Trie, Merge_0D) {

    Trie<0> e;
    Trie<0> f;
    f.insert();


    {
        Trie<0> c = e;
        c.insertAll(e);
        EXPECT_TRUE(c.empty());
    }

    {
        Trie<0> c = e;
        c.insertAll(f);
        EXPECT_FALSE(c.empty());
    }

    {
        Trie<0> c = f;
        c.insertAll(e);
        EXPECT_FALSE(c.empty());
    }

    {
        Trie<0> c = f;
        c.insertAll(f);
        EXPECT_FALSE(c.empty());
    }

}

TEST(Trie, Merge_1D) {

    Trie<1> e;
    Trie<1> a;
    Trie<1> b;

    for(int i=0; i<5; i++) {
        a.insert(i);
        b.insert(i+5);
    }

    {
        Trie<1> c = e;
        c.insertAll(a);
        for(int i=0; i<10; i++) {
            EXPECT_EQ(a.contains(i), c.contains(i));
        }
    }

    {
        Trie<1> c = e;
        c.insertAll(b);
        for(int i=0; i<10; i++) {
            EXPECT_EQ(b.contains(i), c.contains(i));
        }
    }

    {
        Trie<1> c = e;
        c.insertAll(a);
        c.insertAll(b);
        for(int i=0; i<10; i++) {
            EXPECT_EQ(a.contains(i) || b.contains(i), c.contains(i));
        }
    }
}

TEST(Trie, Merge_2D) {

    Trie<2> e;
    Trie<2> a;
    Trie<2> b;

    for(int i=0; i<5; i++) {
        for(int j=0; j<5; j++) {
            a.insert(i,j);
            b.insert(i+5,j+5);
        }
    }

    {
        Trie<2> c = e;
        c.insertAll(a);
        for(int i=0; i<10; i++) {
            for(int j=0; j<10; j++) {
                EXPECT_EQ(a.contains(i,j), c.contains(i,j));
            }
        }
    }

    {
        Trie<2> c = e;
        c.insertAll(b);
        for(int i=0; i<10; i++) {
            for(int j=0; j<10; j++) {
                EXPECT_EQ(b.contains(i,j), c.contains(i,j));
            }
        }
    }

    {
        Trie<2> c = e;
        c.insertAll(a);
        c.insertAll(b);
        for(int i=0; i<10; i++) {
            for(int j=0; j<10; j++) {
                EXPECT_EQ(a.contains(i,j) || b.contains(i,j), c.contains(i,j));
            }
        }
    }
}

TEST(Trie, Merge_3D) {

    Trie<3> e;
    Trie<3> a;
    Trie<3> b;

    for(int i=0; i<5; i++) {
        for(int j=0; j<5; j++) {
            for(int k=0; k<5; k++) {
                a.insert(i,j,k);
                b.insert(i+5,j+5,k+5);
            }
        }
    }

    {
        Trie<3> c = e;
        c.insertAll(a);
        for(int i=0; i<10; i++) {
            for(int j=0; j<10; j++) {
                for(int k=0; k<5; k++) {
                    EXPECT_EQ(a.contains(i,j,k), c.contains(i,j,k));
                }
            }
        }
    }

    {
        Trie<3> c = e;
        c.insertAll(b);
        for(int i=0; i<10; i++) {
            for(int j=0; j<10; j++) {
                for(int k=0; k<5; k++) {
                    EXPECT_EQ(b.contains(i,j,k), c.contains(i,j,k));
                }
            }
        }
    }

    {
        Trie<3> c = e;
        c.insertAll(a);
        c.insertAll(b);
        for(int i=0; i<10; i++) {
            for(int j=0; j<10; j++) {
                for(int k=0; k<5; k++) {
                    EXPECT_EQ(a.contains(i,j,k) || b.contains(i,j,k), c.contains(i,j,k));
                }
            }
        }
    }
}

TEST(Trie, Merge_Stress) {

    typedef typename Trie<2>::entry_type entry_t;

    const int N = 1000;
    const int M = 100;

    std::set<entry_t> ref;
    Trie<2> a;

    for(int i=0; i<M; i++) {

        Trie<2> b;
        for(int i=0; i<N; i++) {
            RamDomain x = rand()%(N/2);
            RamDomain y = rand()%(N/2);
            if(!a.contains(x,y)) {
                b.insert(x,y);
                ref.insert(entry_t({{x,y}}));
            }
        }

        a.insertAll(b);

        std::set<entry_t> is(a.begin(), a.end());
        std::set<entry_t> should(ref.begin(), ref.end());
        EXPECT_EQ(should, is);
    }

}

TEST(Trie, Merge_Bug) {

    // having this set ...
    Trie<2> a;
    a.insert(25129,67714);
    a.insert(25132,67714);
    a.insert(84808,68457);

//    a.getStore().dump();

    // ... merged with an empty set ...
    Trie<2> b;
    a.insertAll(b);

//    a.getStore().dump();

    // and later on merged with a third set
    Trie<2> c;
    c.insert(133,455);
    c.insert(10033,455);
    a.insertAll(c);

//    a.getStore().dump();

    // ... caused the first element to be missing in the iterator
    int count = 0;
    for(auto it = a.begin(); it != a.end(); ++it) {
        count++;
    }

    // if there are 5 elements, everything is fine
    EXPECT_EQ(5,count);
}

TEST(Trie,Size) {

    Trie<2> t;

    EXPECT_TRUE(t.empty());
    EXPECT_EQ(0, t.size());

    t.insert(1,2);

    EXPECT_FALSE(t.empty());
    EXPECT_EQ(1, t.size());

    t.insert(1,2);

    EXPECT_FALSE(t.empty());
    EXPECT_EQ(1, t.size());

    t.insert(2,1);

    EXPECT_FALSE(t.empty());
    EXPECT_EQ(2, t.size());


    Trie<2> t2;

    t2.insert(1,2);
    t2.insert(1,3);
    t2.insert(1,4);
    t2.insert(3,2);

    EXPECT_EQ(4,t2.size());

    t.insertAll(t2);
    EXPECT_FALSE(t.empty());
    EXPECT_EQ(5, t.size());

}

TEST(Trie,Limits) {

    Trie<2> data;

    EXPECT_EQ(0, data.size());
    data.insert(10,15);
    EXPECT_EQ(1, data.size());

    data.insert((1<<31) + (1<<30), 18);
    EXPECT_EQ(2, data.size());

    Trie<2> a;
    a.insert(140,15);

    Trie<2> b;
    b.insert(25445, 18);

    b.insertAll(a);

    EXPECT_EQ(2, b.size());

    int counter = 0;
    for(auto it = b.begin(); it != b.end(); ++it) {
        counter++;
    }
    EXPECT_EQ(2,counter);
}

TEST(Trie,Parallel) {

    const int N = 10000;

    // get a unordered list of test data
    typedef typename Trie<2>::entry_type entry_t;
    std::vector<entry_t> list;
    Trie<2> filter;

    while(filter.size() < N) {
        entry_t entry({{(RamDomain)(random() % N), (RamDomain)(random() % N)}});
        if (filter.insert(entry)) {
            list.push_back(entry);
        }
    }

    // the number of times duplicates show up in the input set
    for(int dup = 1; dup < 4; dup++) {

        // now duplicate this list
        std::vector<entry_t> full;
        for(int i=0; i<dup; i++) {
            for(const auto& cur : list) {
                full.push_back(cur);
            }
        }

        // shuffle data
        std::random_shuffle(full.begin(), full.end());

        // now insert all those values into a new set - in parallel
        Trie<2> res;
        #pragma omp parallel for
        for(std::vector<entry_t>::iterator it = full.begin(); it < full.end(); ++it) {
            res.insert(*it);
        }

        // check resulting values
        EXPECT_EQ(N,res.size());

        std::set<entry_t> should(full.begin(), full.end());
        std::set<entry_t> is(res.begin(), res.end());

        for(const auto& cur : should) {
            EXPECT_TRUE(res.contains(cur)) << "Missing: " << cur << "\n";
        }

        for(const auto& cur : res) {
            EXPECT_TRUE(should.find(cur) != should.end())
                    << "Additional: " << cur << "\n"
                    << "Contained: " << res.contains(cur) << "\n";
        }

        std::vector<entry_t> extra;
        for(const auto& cur : is) {
            if (should.find(cur) == should.end()) extra.push_back(cur);
        }
        EXPECT_TRUE(extra.empty()) << "Extra elments: " << extra << "\n";

        std::vector<entry_t> missing;
        for(const auto& cur : should) {
            if (is.find(cur) == is.end()) missing.push_back(cur);
        }
        EXPECT_TRUE(missing.empty())
            << "Missing elments: " << missing << "\n"
            << "All Elements: " << should << "\n";

        EXPECT_EQ(N,should.size());
        EXPECT_EQ(N,is.size());
        EXPECT_EQ(should,is);
    }

}
