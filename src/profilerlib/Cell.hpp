/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

#pragma once

#include <string>
#include <iostream>
#include "StringUtils.hpp"
#include "CellInterface.hpp"


template <typename T>
class Cell : public CellInterface {
public:
    T val;
    Cell(T value) : val(value) {};
};

template <>
class Cell<double> : public CellInterface {
public:
    double val;
    Cell(double value) : val(value) {};
    double getDoubVal() {return val;}
    long getLongVal() {std::cerr<< "getting long on double cell\n"; throw this;}
    std::string getStringVal() {std::cerr << "getting string on double cell\n"; throw this;}
    std::string toString(int precision) {
        return Tools::formatTime(val);
    }
};

template <>
class Cell<std::string> : public CellInterface {
public:
    std::string val;
    Cell(std::string value) : val(value) {};
    double getDoubVal() {std::cerr << "getting double on string cell\n"; throw this;}
    long getLongVal() {std::cerr << "getting long on string cell\n"; throw this;}
    std::string getStringVal() { return val; }
    std::string toString(int precision) { return Tools::cleanString(val); }
};

template <>
class Cell<long> : public CellInterface {
public:
    long val;
    Cell(long value) : val(value) {};
    double getDoubVal() {std::cerr << "getting double on long cell\n"; throw this;}
    std::string getStringVal() {std::cerr << "getting string on long cell\n"; throw this;}
    long getLongVal() {return val;}
    std::string toString(int precision) {
        return Tools::formatNum(precision, val);
    };
};

template<>
class Cell<void> : public CellInterface, std::false_type {
public:
    Cell(void){};
    double getDoubVal() {std::cerr << "getting double on void cell"; throw this;}
    long getLongVal() {std::cerr << "getting long on void cell"; throw this;}
    std::string getStringVal() {std::cerr << "getting string on void cell\n"; throw this;}
    std::string toString(int precision) {return "-";}
};
