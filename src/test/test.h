/*
 * Copyright (c) 2013, 2014, Oracle and/or its affiliates. All Rights reserved
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
 * @file test.h
 *
 * Simple unit test infrastructure
 *
 ***********************************************************************/
#pragma once 
#include <iostream>
#include <fstream>
#include <string>
#include <set>

/* singly linked list for linking test caes */ 

static class TestCase *base = nullptr;

class TestCase {
private:
    TestCase *next;        // next test case (linked by constructor) 
    std::string group;     // group name of test 
    std::string test;      // test name of test 
    size_t num_checks;     // number of checks 
    size_t num_failed;     // number of failed checks 
    std::ofstream logfile; // logfile 

public:
    TestCase(std::string g, std::string t) : group(g), test(t), num_checks(0), num_failed(0) {
        next = base; 
        base = this; 
        logfile.open("logfile_" __FILE__ ".txt");
    } 
    virtual ~TestCase() { 
    }

    /**
     * Checks condition
     * 
     * checks whether condition holds and update counters. In case of failed check, the 
     * failed check is logged. 
     */ 
    std::ostream & expect(bool condition, const std::string &txt, const std::string &loc) {
        num_checks ++;
        if (!condition) { 
            num_failed++;
            logfile << "\nFAILED ";
            logfile << loc << " condition: " << txt << "\n";
            logfile << "\t";
        }
        return logfile;
    }

    /**
     * Fatal condition
     * 
     * Same as check() except in case of a condition that evaluates to false, the method
     * aborts the test. 
     */ 
    std::ostream & fatal(bool condition, const std::string &txt, const std::string &loc) {
        if (!condition) { std::cerr << "Tests failed.\n"; exit(99); } 
        return expect(condition, txt, loc); 
    }

    /**
     * Run method 
     */ 
    virtual void run() = 0; 

    /**
     * Next Test Case in singly linked list 
     */ 
    TestCase *nextTestCase() { 
        return next;
    } 

    /**
     * get test name
     */ 
    const std::string &getTest() const {
        return test; 
    } 

    /**
     * get name of test group 
     */ 
    const std::string &getGroup() const {
        return group;
    } 

    /**
     * get number of checks
     */ 
    size_t getChecks() const {
        return num_checks;
    } 

    /** 
     * get number of failed checks 
     */ 
    size_t getFailed() const {
        return num_failed;
    } 
};

#define TEST(a,b) class test_##a##_##b : public TestCase { \
    public: \
        test_##a##_##b(std::string g, std::string t) : TestCase(g,t) { } \
    void run(); \
} Test_##a##_##b(#a,#b); \
void test_##a##_##b::run()

#define S(x) #x
#define S_(x) S(x)
#define S__LINE__ S_(__LINE__)

#define LOC __FILE__ ":" S__LINE__
#define EXPECT_TRUE(a) expect(a, #a, LOC) 
#define EXPECT_FALSE(a) expect(!(a), "NOT " #a, LOC )
#define EXPECT_EQ(a,b) expect((a)==(b), "EQ(" #a "," #b ")", LOC)
#define EXPECT_LT(a,b) expect((a)<(b), "EQ(" #a "," #b ")", LOC )
#define EXPECT_NE(a,b) expect((a)!=(b), "NE(" #a "," #b ")", LOC)
#define EXPECT_STREQ(a,b) expect(std::string(a)==std::string(b), "EQ(" #a "," #b ")", LOC)
#define EXPECT_PRED2(p,a,b) expect(p(a,b), #p "(" #a "," #b ")", LOC)

#define ASSERT_TRUE(a) fatal(a, #a, LOC) 
#define ASSERT_LE(a,b) fatal((a)<=(b), "LE(" #a "," #b ")", LOC )

/**
 * Main program of a unit test
 */ 

int main(int argc, char **argv) {

    // add all groups to a set 
    std::set<std::string> groups; 
    for(TestCase *p=base;p!=nullptr;p=p->nextTestCase()) {
        groups.insert(p->getGroup()); 
    } 

    // traverse groups and execute associated test cases 
    int failure = 0; 
    for(auto &group : groups) {
        std::cout << group << "\n";
        for(TestCase *p=base;p!=nullptr;p=p->nextTestCase()) {
            if(p->getGroup() == group) { 
                p->run(); 
                std::cerr << "\t" << ((p->getFailed() == 0)?"OK":"FAILED");
                std::cerr << " (" << p->getChecks()- p->getFailed();
                std::cerr << "/" << p->getChecks();
                std::cerr  << ")\t" << p->getTest() << "\n";
                if (p->getFailed() != 0) {
                    failure = 99;
                } 
            } 
        }
    } 
    if(failure != 0) { 
        std::cerr << "Tests failed.\n";
    }
    return failure; 
}
