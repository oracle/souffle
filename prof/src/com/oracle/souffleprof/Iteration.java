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
import java.util.HashMap;
import java.util.Map;

/***
 * Profile Data Model
 * 
 * Represents recursive profile data 
 */

public class Iteration implements Serializable {

	private static final long serialVersionUID = 4513587123648922999L;
	private double runtime = 0;
	private long num_tuples = 0;
	private double copy_time = 0;
	private String locator = "";
	private long prev_num_tuples = 0;

	private Map<String, RuleRecursive> rul_rec_map;

	public Iteration() {
		this.rul_rec_map = new HashMap<String, RuleRecursive>();
	}

	public void addRule(String[] data, String rec_id) {
		String strTemp = data[4] + data[3] + data[2];

		if (data[0].charAt(0) == 't') {
			boolean exists = false;

			for (String name : rul_rec_map.keySet()) {
				if (name.equals(strTemp)) {
					RuleRecursive rul_rec = rul_rec_map.get(strTemp);
					rul_rec.setRuntime(Double.parseDouble(data[5])
							+ rul_rec.getRuntime());
					exists = true;
				}
			}
			if (!exists) {
				RuleRecursive rul_rec = new RuleRecursive(data[4],
						Integer.parseInt(data[2]), rec_id);
				rul_rec.setRuntime(Double.parseDouble(data[5]));
				rul_rec.setLocator(data[3]);
				rul_rec_map.put(strTemp, rul_rec);
			}

		} else if (data[0].charAt(0) == 'n') {
			RuleRecursive rul_rec = rul_rec_map.get(strTemp);
			assert rul_rec != null : "missing t tag";
			rul_rec.setNum_tuples(Long.parseLong(data[5]) - prev_num_tuples);
			this.prev_num_tuples = Long.parseLong(data[5]);
			rul_rec_map.put(strTemp, rul_rec);
		}
	}

	public Map<String, RuleRecursive> getRul_rec() {
		return rul_rec_map;
	}

	@Override
	public String toString() {

		StringBuilder res = new StringBuilder();
		res.append("" + runtime + "," + num_tuples + "," + copy_time + ",");
		res.append(" recRule:");
		for (RuleRecursive rul : rul_rec_map.values()) {
			res.append(rul.toString());
		}
		res.append("\n");
		return res.toString();
	}

	public double getRuntime() {
		return runtime;
	}

	public void setRuntime(double runtime) {
		this.runtime = runtime;
	}

	public long getNum_tuples() {
		return num_tuples;
	}

	public void setNum_tuples(long num_tuples) {
		this.num_tuples = num_tuples;
	}

	public double getCopy_time() {
		return copy_time;
	}

	public void setCopy_time(double copy_time) {
		this.copy_time = copy_time;
	}

	public String getLocator() {
		return locator;
	}

	public void setLocator(String locator) {
		this.locator = locator;
	}
}
