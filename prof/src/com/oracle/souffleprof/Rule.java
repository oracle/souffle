/*
 * Souffle version 0.0.0
 * Copyright (c) 2015, Oracle and/or its affiliates. All rights reserved
 * Licensed under the Universal Permissive License v 1.0 as shown at:
 * - https://opensource.org/licenses/UPL
 * - souffle/LICENSE
 */

package com.oracle.souffleprof;

import java.io.Serializable;

public class Rule implements Serializable {

    private static final long serialVersionUID = -2518455587078964434L;
    protected String name;
    protected double runtime = 0;
    protected long num_tuples = 0;
    protected String id;
    protected String locator = "";

    public Rule(String name, String id) {
        this.name = name;
        this.id = id;
    }

    public String getId() {
        return id;
    }

    public double getRuntime() {
        return runtime;
    }

    public long getNum_tuples() {
        return num_tuples;
    }

    public void setRuntime(double runtime) {
        this.runtime = runtime;
    }

    public void setNum_tuples(long num_tuples) {
        this.num_tuples = num_tuples;
    }

    public String getName() {
        return name;
    }

    public void setId(String id) {
        this.id = id;
    }

    public String getLocator() {
        return locator;
    }

    public void setLocator(String locator) {
        if (this.locator.isEmpty()) {
            this.locator = locator;
        } else {
            this.locator += " " + locator;
        }

    }

    @Override
    public String toString() {
        StringBuilder result = new StringBuilder();
        result.append("{" + name + ":");
        result.append("[" + runtime + "," + num_tuples + "]");
        result.append("}");
        return result.toString();
    }

}
