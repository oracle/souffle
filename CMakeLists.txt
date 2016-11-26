cmake_minimum_required(VERSION 3.3)
project(cpp_prof_souffle)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
    src/profilerlib/Cli.hpp
    src/profilerlib/Iteration.hpp
    src/profilerlib/OutputProcessor.hpp
    src/profilerlib/ProgramRun.hpp
    src/profilerlib/Reader.hpp
    src/profilerlib/Relation.hpp
    src/profilerlib/Rule.hpp
    src/profilerlib/Tui.hpp
    src/Makefile.am
    src/souffle_prof.cpp
    Makefile.am src/profilerlib/Reader.cpp src/profilerlib/Relation.cpp src/profilerlib/Rule.cpp src/profilerlib/Iteration.cpp)

add_executable(cpp_prof_souffle ${SOURCE_FILES})