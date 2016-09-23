# Souffle - A Datalog Compiler
# (c) 2016 by the Souffle Development Team
# Licensed under the Universal Permissive License v 1.0 as shown at:
# - https://opensource.org/licenses/UPL
# - <souffle root>/licenses/SOUFFLE-UPL.txt
if JAVAI

SUFFIXES = .cpp .h .cc .o .a .class .java

DIR = @abs_top_srcdir@/interfaces
GENDIR = @abs_top_srcdir@/interfaces/jni/include/generated
dist_bin_SCRIPTS = com.soufflelang.souffle.jar souffle-compilelib

java_jni_classes = com/soufflelang/souffle/Result.class \
	com/soufflelang/souffle/Data.class \
	com/soufflelang/souffle/PrimData.class \
	com/soufflelang/souffle/Arg.class \
	com/soufflelang/souffle/SConst.class \
	com/soufflelang/souffle/Const.class \
	com/soufflelang/souffle/Var.class \
	com/soufflelang/souffle/Executor.class \
	com/soufflelang/souffle/BinOp.class \
	com/soufflelang/souffle/Atom.class \
	com/soufflelang/souffle/Literal.class \
	com/soufflelang/souffle/Negation.class \
	com/soufflelang/souffle/Constraint.class \
	com/soufflelang/souffle/Clause.class \
	com/soufflelang/souffle/Program.class \
	com/soufflelang/souffle/Relation.class \
	com/soufflelang/souffle/PrimType.class \
	com/soufflelang/souffle/Solver.class

JSRC = $(wildcard ./jni/java/*.java)

$(java_jni_classes): $(JSRC)
	$(JAVAC) -d $(DIR) $(JSRC)
	$(JAR) -cf com.soufflelang.souffle.jar ./com/soufflelang/souffle/*.class

com_soufflelang_souffle_Result.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.Result

com_soufflelang_souffle_Data.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.Data

com_soufflelang_souffle_PrimData.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.PrimData

com_soufflelang_souffle_Arg.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.Arg

com_soufflelang_souffle_SConst.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.SConst

com_soufflelang_souffle_Const.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.Const

com_soufflelang_souffle_Var.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.Var

com_soufflelang_souffle_Executor.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.Executor

com_soufflelang_souffle_BinOp.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.BinOp

com_soufflelang_souffle_Atom.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.Atom

com_soufflelang_souffle_Literal.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.Literal

com_soufflelang_souffle_Negation.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.Negation

com_soufflelang_souffle_Constraint.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.Constraint

com_soufflelang_souffle_Clause.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.Clause

com_soufflelang_souffle_Program.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.Program

com_soufflelang_souffle_Relation.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.Relation

com_soufflelang_souffle_PrimType.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.PrimType

com_soufflelang_souffle_Solver.h: $(java_jni_classes)
	$(JAVAH) -jni -d $(DIR) com.soufflelang.souffle.Solver

nodist_libsoufflejni_la_SOURCES = com_soufflelang_souffle_Arg.h com_soufflelang_souffle_Const.h com_soufflelang_souffle_Literal.h com_soufflelang_souffle_Program.h com_soufflelang_souffle_Solver.h com_soufflelang_souffle_Atom.h com_soufflelang_souffle_Constraint.h com_soufflelang_souffle_Negation.h com_soufflelang_souffle_Relation.h com_soufflelang_souffle_Var.h com_soufflelang_souffle_BinOp.h com_soufflelang_souffle_Data.h com_soufflelang_souffle_PrimData.h com_soufflelang_souffle_Result.h com_soufflelang_souffle_Clause.h com_soufflelang_souffle_Executor.h com_soufflelang_souffle_PrimType.h com_soufflelang_souffle_SConst.h

CLEANFILES = com_soufflelang_souffle_Arg.h com_soufflelang_souffle_Const.h com_soufflelang_souffle_Literal.h com_soufflelang_souffle_Program.h com_soufflelang_souffle_Solver.h com_soufflelang_souffle_Atom.h com_soufflelang_souffle_Constraint.h com_soufflelang_souffle_Negation.h com_soufflelang_souffle_Relation.h com_soufflelang_souffle_Var.h com_soufflelang_souffle_BinOp.h com_soufflelang_souffle_Data.h com_soufflelang_souffle_PrimData.h com_soufflelang_souffle_Result.h com_soufflelang_souffle_Clause.h com_soufflelang_souffle_Executor.h com_soufflelang_souffle_PrimType.h com_soufflelang_souffle_SConst.h

BUILT_SOURCES = com_soufflelang_souffle_Arg.h com_soufflelang_souffle_Const.h com_soufflelang_souffle_Literal.h com_soufflelang_souffle_Program.h com_soufflelang_souffle_Solver.h com_soufflelang_souffle_Atom.h com_soufflelang_souffle_Constraint.h com_soufflelang_souffle_Negation.h com_soufflelang_souffle_Relation.h com_soufflelang_souffle_Var.h com_soufflelang_souffle_BinOp.h com_soufflelang_souffle_Data.h com_soufflelang_souffle_PrimData.h com_soufflelang_souffle_Result.h com_soufflelang_souffle_Clause.h com_soufflelang_souffle_Executor.h com_soufflelang_souffle_PrimType.h com_soufflelang_souffle_SConst.h

# cpp files
libsoufflejni_la_SOURCES = ./jni/src/Negation.cpp \
				./jni/src/Constraint.cpp \
				./jni/src/Data.cpp  \
				./jni/src/Arg.cpp \
				./jni/src/PrimData.cpp \
				./jni/src/Result.cpp \
				./jni/src/Var.cpp \
				./jni/src/BinOp.cpp \
				./jni/src/Const.cpp \
				./jni/src/Executor.cpp \
				./jni/src/SConst.cpp \
				./jni/src/Atom.cpp \
				./jni/src/Clause.cpp \
				./jni/src/Program.cpp \
				./jni/src/Relation.cpp \
				./jni/src/PrimType.cpp \
				./jni/src/Solver.cpp \
				./src/AstBuilder.cpp \
				./src/Interfaces.cpp

#libsoufflejni_la_CXXFLAGS=-I./jni/include/ -I./jni/include/generated/ -I./include/ -I/usr/lib/jvm/java-8-oracle/include/linux/ -I/usr/lib/jvm/java-8-oracle/include/ -I@abs_top_srcdir@/src/ -I@abs_top_srcdir@/ -I@abs_top_srcdir@/include/ -I./include/ $(JNIFLAGS)


libsoufflejni_la_CXXFLAGS= -I$(JAVA_HOME)/include/ -I$(JAVA_HOME)/include/darwin/ -I./jni/include/ -I./jni/include/generated/ -I./include/ -I@abs_top_srcdir@/src/ -I@abs_top_srcdir@/ -I@abs_top_srcdir@/include/ -I./include/ $(JNIFLAGS) 

libsoufflejni_la_LIBADD = @abs_top_srcdir@/src/libsouffle.a

lib_LTLIBRARIES = libsoufflejni.la
endif
