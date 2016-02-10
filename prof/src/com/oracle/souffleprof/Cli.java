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

/**
 * Command Line Interface 
 * 
 * Provides an interface to parse the execution arguments of the souffle profiler 
 */

public class Cli {

    /**
     * List of arguments
     */
    private String[] args;

    /**
     * Constructor for the command line interface
     *  
     * @param args arguments of the static main functions
     */
    public Cli(String[] args) {
        this.args = args;
    }

    /**
     * Error handler
     * 
     * Prints usage of souffle profiler
     */
    public void error() {
        System.out.println("java -jar souffleprof.jar [-f <file> [-c <command>] [-l]] [-h] [-v]"); 
        System.exit(1); 
    }


    /**
     * Parse command line parameters 
     * 
     * Arguments are checked and the text-user interface of the profiler is invoked.
     */
    public void parse() {
        /** 
         * Profile command provided by argument interface 
         */ 
        String [] commands = {};

        /**
         * Filename 
         */ 
        String filename = "";

        /** 
         * Alive flag denoting whether a completed profile is processed or a running one 
         */ 
        boolean alive = false;

        int i=0;

        while (i < args.length && args[i].startsWith("-")) {
            String arg = args[i++]; 

            if (arg.equals("-h")) { 
                System.out.println("SouffleProf Alpha3 (2 Feb 2015)");
                System.out.println("");
                error(); 
            } else if (arg.equals("-c")) { 
                if (i  <= args.length) { 
                    commands = args[i++].split("\\s+"); 
                } else { 
                    System.out.println("Parameter for option -c missing!"); 
                    error(); 
                }
            } else if (arg.equals("-f")) { 
                if (i  <= args.length) { 
                    filename = args[i++]; 
                } else { 
                    System.out.println("Parameter for option -f missing!"); 
                    error(); 
                }
            } else if (arg.equals("-l")) {
                alive = true; 
            } else {
                System.out.println("Unknown argument " + args[i]); 
                error(); 
            }
        }

        /**
         * Invoke text user interface
         */
        if (commands.length > 0) { 
            new Tui(filename, alive).runCommand(commands); 
        } else {
            new Tui(filename, alive).runProf(); 
        }
    }
}
