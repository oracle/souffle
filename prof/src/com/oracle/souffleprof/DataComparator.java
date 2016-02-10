/*
 * Copyright (c) 2015, Oracle and/or its affiliates. All Rights reserved
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

package com.oracle.souffleprof;

import java.util.Comparator;

/**
 * Compare operation for profile data for sorting data according to a key
 */
public enum DataComparator implements Comparator<Object[]> {
    TIME {
        public int compare(Object[] a, Object[] b) {
        	return Double.compare((Double)b[0], (Double)a[0]);
        }},
    NR_T {
        public int compare(Object[] a, Object[] b) {
        	return Double.compare((Double)b[1], (Double)a[1]);
        }},
    R_T {
        public int compare(Object[] a, Object[] b) {
        	return Double.compare((Double)b[2], (Double)a[2]);
        }},     
    C_T {
        public int compare(Object[] a, Object[] b) {
        	return Double.compare((Double)b[3], (Double)a[3]);
        }},
    TUP {
        public int compare(Object[] a, Object[] b) {
        	return Long.compare((Long)b[4], (Long)a[4]);
        }},
    NAME {
        public int compare(Object[] a, Object[] b) {
        	String aStr = (String)a[5];
        	String bStr = (String)b[5];
        	return Integer.valueOf(aStr.compareTo(bStr));
        }},
    PER {
        public int compare(Object[] a, Object[] b) {
        	return Double.compare((Double)b[8], (Double)a[8]);
        }};

        public static Comparator<Object[]> decending(final Comparator<Object[]> other) {
            return new Comparator<Object[]>() {
                public int compare(Object[] o1, Object[] o2) {
                    return -1 * other.compare(o1, o2);
                }
            };
        }

        public static Comparator<Object[]> getComparator(final int sortDir, final DataComparator... multipleOptions) {
            return new Comparator<Object[]>() {
                public int compare(Object[] o1, Object[] o2) {
                    for (DataComparator option : multipleOptions) {
                        int result = option.compare(o1, o2);
                        if (result != 0) {
                            return sortDir * result;
                        }
                    }
                    return 0;
                }
            };
        }
}