/**
 * Copyright (c) 2015, Oracle and/or its affiliates.
 *
 * All rights reserved.
 */

package com.oracle.souffleprof;


/**
 * Souffle main class calling the command line interface that 
 * further invokes the text user interface.
 */
public class SouffleProf {
    public static void main(String[] args) throws InterruptedException {
        new Cli(args).parse();
        System.out.print("\r");
    }
}
