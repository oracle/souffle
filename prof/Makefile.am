# Souffle - A Datalog Compiler
# Copyright (c) 2013-15, Oracle and/or its affiliates. All rights reserved.
# Licensed under the Universal Permissive License v 1.0 as shown at:
# - https://opensource.org/licenses/UPL
# - <souffle root>/licenses/SOUFFLE-UPL.txt

DIR = @abs_top_srcdir@/prof
dist_bin_SCRIPTS = souffle-profile souffleprof.jar
JSRC = $(wildcard src/com/oracle/souffleprof/*.java)

souffleprof.jar: $(JSRC)
	$(JAVAC) -encoding UTF-8 -d ./ -sourcepath $(DIR)/src $(DIR)/src/com/oracle/souffleprof/SouffleProf.java
	$(JAR) cvfm souffleprof.jar $(srcdir)/MANIFEST.MF com/


EXTRA_DIST = $(JSRC) ./MANIFEST.MF

clean::
	rm -rf com souffleprof.jar
