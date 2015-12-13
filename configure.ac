# Copyright (c) 2013, Oracle and/or its affiliates.
#
# All rights reserved.

AC_INIT(souffle, 1.0, [programanalysis_au_grp@oracle.com])
AC_PREREQ(2.68)
AC_COPYRIGHT(['2013-15 Oracle and/or its affiliates'])
AC_CONFIG_MACRO_DIRS([m4])

AC_CANONICAL_TARGET # target_cpu, target_vendor, and target_os
AC_CANONICAL_BUILD  # build_cpu, build_vendor, and build_os
AC_CANONICAL_HOST   # host_cpu, host_vendor, and host_os

AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_MAINTAINER_MODE
AC_CONFIG_HEADERS(config.h)

dnl Do not set default CFLAGS and CXXFLAGS
CXXFLAGS=" -O3 -Wall -std=c++11  $CXXFLAGS"

AC_ISC_POSIX
AC_PROG_CPP
AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AM_PROG_CC_STDC
AC_PROG_RANLIB
AX_PROG_JAVAC
AX_PROG_JAVA
AX_PROG_JAR

# Check fol flex
AC_CHECK_PROG(FLEX, flex, flex)
AS_IF([test -z "$FLEX"], [AC_MSG_ERROR([flex not found.])])

# Check for bison
AC_CHECK_PROG(BISON, bison, bison)
AS_IF([test -z "$BISON"], [AC_MSG_ERROR([bison not found.])])

dnl Enable pthread library
AC_CHECK_LIB(pthread, pthread_create,,
          [AC_MSG_ERROR([required library pthread missing])])

dnl Enables OpenMP in the souffle compiler and interpreter
AC_OPENMP
AS_VAR_APPEND(CXXFLAGS, [" $OPENMP_CFLAGS "])

AC_CONFIG_TESTDIR([tests])
AC_CONFIG_FILES([
    Makefile
    src/Makefile
    prof/Makefile
    tests/Makefile
    tests/atlocal
    src/souffle-compile
    src/souffle-config
])
AC_CONFIG_LINKS([include/souffle/CompiledRamOptions.h:src/CompiledRamOptions.h])
AC_CONFIG_LINKS([include/souffle/CompiledRamRelation.h:src/CompiledRamRelation.h])
AC_CONFIG_LINKS([include/souffle/CompiledRamTuple.h:src/CompiledRamTuple.h])
AC_CONFIG_LINKS([include/souffle/CompiledRamRecord.h:src/CompiledRamRecord.h])
AC_CONFIG_LINKS([include/souffle/CompiledSouffle.h:src/CompiledSouffle.h])
AC_CONFIG_LINKS([include/souffle/SouffleInterface.h:src/SouffleInterface.h])
AC_CONFIG_LINKS([include/souffle/ParallelUtils.h:src/ParallelUtils.h])
AC_CONFIG_LINKS([include/souffle/RamTypes.h:src/RamTypes.h])
AC_CONFIG_LINKS([include/souffle/BTree.h:src/BTree.h])
AC_CONFIG_LINKS([include/souffle/Trie.h:src/Trie.h])
AC_CONFIG_LINKS([include/souffle/Table.h:src/Table.h])
AC_CONFIG_LINKS([include/souffle/IterUtils.h:src/IterUtils.h])
AC_CONFIG_LINKS([include/souffle/Util.h:src/Util.h])
AC_CONFIG_LINKS([include/souffle/RamLogger.h:src/RamLogger.h])
AC_CONFIG_LINKS([include/souffle/SymbolTable.h:src/SymbolTable.h])
AC_CONFIG_LINKS([prof/souffle-profile:prof/souffle-profile])

AM_MISSING_PROG([AUTOM4TE], [autom4te])

# Checks for header files.
AC_FUNC_ALLOCA
AC_CHECK_HEADERS([arpa/inet.h fcntl.h float.h inttypes.h libintl.h limits.h malloc.h memory.h netdb.h stddef.h stdint.h stdlib.h string.h strings.h sys/time.h sys/timeb.h unistd.h wchar.h wctype.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_C_INLINE
AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_FUNC_MMAP
AC_FUNC_REALLOC
AC_FUNC_STRERROR_R
AC_CHECK_FUNCS([dup2 fchdir getcwd getpagesize gettimeofday isascii memset mkdir munmap pow regcomp rmdir setenv socket strcasecmp strchr strdup strerror strrchr strstr strtol strtoull])
AC_OUTPUT
