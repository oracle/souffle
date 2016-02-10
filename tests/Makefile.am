# Copyright (c) 2013, 2015, Oracle and/or its affiliates. All rights reserved.
#
# The Universal Permissive License (UPL), Version 1.0
#
# Subject to the condition set forth below, permission is hereby granted to any person obtaining a copy of this software,
# associated documentation and/or data (collectively the "Software"), free of charge and under any and all copyright rights in the 
# Software, and any and all patent rights owned or freely licensable by each licensor hereunder covering either (i) the unmodified 
# Software as contributed to or provided by such licensor, or (ii) the Larger Works (as defined below), to deal in both
# 
# (a) the Software, and
# (b) any piece of software and/or hardware listed in the lrgrwrks.txt file if one is included with the Software (each a “Larger
# Work” to which the Software is contributed by such licensors),
#
# without restriction, including without limitation the rights to copy, create derivative works of, display, perform, and 
# distribute the Software and make, use, sell, offer for sale, import, export, have made, and have sold the Software and the 
# Larger Work(s), and to sublicense the foregoing rights on either these or other terms.
#
# This license is subject to the following condition:
# The above copyright notice and either this complete permission notice or at a minimum a reference to the UPL must be included in 
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
# OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
# LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
# IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

EXTRA_DIST =  $(srcdir)/*.at package.m4 $(TESTSUITE) atlocal.in $(srcdir)/evaluation $(srcdir)/semantic $(srcdir)/syntactic $(srcdir)/interface

package.m4: $(top_srcdir)/configure.ac
	@{                                      \
	  echo '# Signature of the current package.'; \
	  echo 'm4_define([AT_PACKAGE_NAME],      [@PACKAGE_NAME@])'; \
	  echo 'm4_define([AT_PACKAGE_TARNAME],   [@PACKAGE_TARNAME@])'; \
	  echo 'm4_define([AT_PACKAGE_VERSION],   [@PACKAGE_VERSION@])'; \
	  echo 'm4_define([AT_PACKAGE_STRING],    [@PACKAGE_STRING@])'; \
	  echo 'm4_define([AT_PACKAGE_BUGREPORT], [@PACKAGE_BUGREPORT@])'; \
	} >package.m4

TESTSUITE = $(srcdir)/testsuite

check-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' $(TESTSUITEFLAGS)

installcheck-local: atconfig atlocal $(TESTSUITE)
	$(SHELL) '$(TESTSUITE)' AUTOTEST_PATH='$(bindir)' \
	$(TESTSUITEFLAGS)

clean-local:
	test ! -f '$(TESTSUITE)' ||  $(SHELL) '$(TESTSUITE)' --clean
	rm -f testsuite
	rm -f package.m4

distclean-local:
	rm -f atconfig

AUTOTEST = $(AUTOM4TE) --language=autotest
$(TESTSUITE): $(srcdir)/testsuite.at $(srcdir)/syntactic.at $(srcdir)/semantic.at $(srcdir)/evaluation.at $(srcdir)/interface.at package.m4
	$(AUTOTEST) -I '$(srcdir)' -o $@.tmp $@.at
	mv $@.tmp $@
