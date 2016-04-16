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
 * @file btree_set_test.h
 *
 * A test case testing the B-trees utilization as sets.
 *
 ***********************************************************************/

#include "test.h"

#include "ParallelUtils.h"

namespace souffle {
namespace test {

    TEST(ParallelUtils,SpinLock) {

        const int N = 1000000;

        SpinLock lock;

        volatile int c =0;

        #pragma omp parallel for num_threads(4)
        for(int i=0; i<N; i++) {
            lock.lock();
            c++;
            lock.unlock();
        }

        EXPECT_EQ(N, c);

    }


    TEST(ParallelUtils,ReadWriteLock) {

        const int N = 1000000;
        const int K = 10;

        ReadWriteLock lock;

        volatile int c =0;

        #pragma omp parallel for num_threads(4)
        for(int i=0; i<N; i++) {
            if (i % K == 0) {          // 10% write probability
                lock.start_write();
                c++;
                lock.end_write();
            } else {
                lock.start_read();
                // nothing to do here ..
                lock.end_read();
            }
        }

        EXPECT_EQ(N/K, c);

    }

    TEST(ParallelUtils,OptimisticReadWriteLock) {

        const int N = 1000000;
        const int K = 10;

        OptimisticReadWriteLock lock;

        volatile int c =0;

        #pragma omp parallel for num_threads(4)
        for(int i=0; i<N; i++) {
            if (i % K == 0) {          // 10% write probability
                lock.start_write();
                c++;
                c++;
                lock.end_write();
            } else {
                bool succ = false;
                do {
                    auto lease = lock.start_read();
                    // nothing to do here ..
                    int x = c;
                    succ = lock.end_read(lease);
                    EXPECT_TRUE((x%2 == 0) || !succ);
                } while (!succ);
            }
        }

        EXPECT_EQ(2 * (N/K), c);

    }

}
} // end namespace souffle
