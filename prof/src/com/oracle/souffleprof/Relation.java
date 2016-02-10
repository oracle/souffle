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

import java.io.Serializable;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class Relation implements Serializable {

    private static final long serialVersionUID = 2674759421227104137L;
    private String name;
    private double runtime = 0;
    private long prev_num_tuples = 0;
    private long num_tuples = 0;
    private String id;
    private String locator;
    private int rul_id = 0;
    private int rec_id = 0;

    private List<Iteration> iterations;
    private Map<String, Rule> ruleMap;

    private boolean ready = true;

    public Relation(String name, String id) {
        this.name = name;
        ruleMap = new HashMap<String, Rule>();
        iterations = new ArrayList<Iteration>();
        this.id = id;
    }

    /**
     * @param data
     *            = [x-rul; rel_name; version; loc; rul_name; val] 
     *            [x-rel; rel_name; loc; val]
     */
    public void addIteration(String[] data) {

        Iteration iter;
        if (ready || iterations.isEmpty()) {
            iter = new Iteration();
            iterations.add(iter);
            ready = false;
        } else {
            iter = iterations.get(iterations.size() - 1);
        }

        if (data[0].contains("rule")) {
            String temp = createRecID(data[4]);
            iter.addRule(data, temp);

        } else if (data[0].charAt(0) == 't' && data[0].contains("relation")) {
            iter.setRuntime(Double.parseDouble(data[3]));
            iter.setLocator(data[2]);
            this.locator = (String) data[2];
        } else if (data[0].charAt(0) == 'n' && data[0].contains("relation")) {
            iter.setNum_tuples(Long.parseLong(data[3]));
        } else if (data[0].charAt(0) == 'c' && data[0].contains("relation")) {
            iter.setCopy_time(Double.parseDouble(data[3]));
            ready = true;
        }

    }

    /*
     * Adds non-recursive rule to this relation.
     */
    public void addRule(String[] data) {
        Rule rul;
        if (!ruleMap.containsKey(data[3])) {
            rul = new Rule(data[3], createID());
            ruleMap.put(data[3], rul);
        } else {
            rul = ruleMap.get(data[3]);
        }

        if (data[0].charAt(0) == 't') {
            // int len = data.length;
            rul.setRuntime(Double.parseDouble(data[4]));
            rul.setLocator(data[2]);
        } else if (data[0].charAt(0) == 'n') {
            assert rul != null;
            rul.setNum_tuples(Long.parseLong(data[4]) - prev_num_tuples);
            this.prev_num_tuples = Long.parseLong(data[4]);
        }

    }

    private String createID() {
        this.rul_id++;
        return "N" + this.id.substring(1) + "." + rul_id;
    }

    private String createRecID(String name) {
        for (Iteration iter : iterations) {
            for (RuleRecursive rul : iter.getRul_rec().values()) {
                if (rul.getName().equals(name)) {
                    return rul.getId();
                }
            }
        }
        this.rec_id++;
        return "C" + this.id.substring(1) + "." + this.rec_id;
    }

    public double getNonRecTime() {
        return this.runtime;
    }

    public double getRecTime() {
        double result = 0;
        for (Iteration iter : iterations) {
            result += iter.getRuntime();
        }
        return result;
    }

    public double getCopyTime() {
        double result = 0;
        for (Iteration iter : iterations) {
            result += iter.getCopy_time();
        }
        return result;
    }

    public long getNum_tuplesRel() {
        Long result = 0L;
        for (Iteration iter : iterations) {
            result += iter.getNum_tuples();
        }
        return this.num_tuples + result;
    }

    public long getNum_tuplesRul() {
        Long result = 0L;
        for (Rule rul : ruleMap.values()) {
            result += rul.getNum_tuples();
        }
        for (Iteration iter : iterations) {
            for (RuleRecursive rul : iter.getRul_rec().values()) {
                result += rul.getNum_tuples();
            }
        }
        return result;
    }

    public Long getTotNum_tuples() {
        return getNum_tuplesRel();
    }

    public Long getTotNumRec_tuples() {
        Long result = 0L;
        for (Iteration iter : iterations) {
            for (RuleRecursive rul : iter.getRul_rec().values()) {
                result += rul.getNum_tuples();
            }
        }
        return result;
    }

    public void setRuntime(double runtime) {
        this.runtime = runtime;
    }

    public void setNum_tuples(long num_tuples) {
        this.num_tuples = num_tuples;
    }

    @Override
    public String toString() {
        StringBuilder result = new StringBuilder();
        result.append("{\n" + name + ":" + runtime + ";" + num_tuples
                + "\n\nonRecRules:\n");
        for (Rule rul : ruleMap.values()) {
            result.append(rul.toString());
        }
        result.append("\n\niterations:\n");
        result.append(iterations.toString());
        result.append("\n}");
        return result.toString();
    }

    public String getName() {
        return this.name;
    }

    /**
     * @return the ruleMap
     */
    public Map<String, Rule> getRuleMap() {
        return this.ruleMap;
    }

    public List<RuleRecursive> getRuleRecList() {
        List<RuleRecursive> temp = new ArrayList<RuleRecursive>();
        for (Iteration iter : iterations) {
            for (RuleRecursive rul : iter.getRul_rec().values()) {
                temp.add(rul);
            }
        }
        return temp;
    }

    public List<Iteration> getIterations() {
        return iterations;
    }

    public String getId() {
        return id;
    }

    public String getLocator() {
        return locator;
    }

    public void setLocator(String locator) {
        this.locator = locator;
    }

}
