# Copyright (c) 2013-15, Oracle and/or its affiliates. All rights reserved.
# Licensed under the Universal Permissive License v 1.0 as shown at:
# - https://opensource.org/licenses/UPL
# - <souffle root>/licenses/SOUFFLE-UPL.txt

dist_bin_SCRIPTS = souffle-profile
nodist_bin_SCRIPTS = souffleprof.jar

souffleprof_src = $(wildcard $(srcdir)/src/com/oracle/souffleprof/*.java)
manifest = $(srcdir)/MANIFEST.MF

souffleprof.jar: $(souffleprof_src)
	$(JAVAC) -encoding UTF-8 -d ./ -sourcepath $(srcdir)/src $(srcdir)/src/com/oracle/souffleprof/SouffleProf.java
	$(JAR) cvfm souffleprof.jar $(manifest) com/

EXTRA_DIST = $(souffleprof_src) $(manifest)

clean-local:
	rm -rf com/ souffleprof.jar
