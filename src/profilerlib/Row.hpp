//
// Created by Dominic Romanowski on 30/11/16.
//

#pragma once

#include <vector>
#include <string>
#include <memory>
#include <iostream>
#include "CellInterface.hpp"

//class emptyCell : CellInterface, std::false_type {
//public:
//    emptyCell(){};
//    double getDoubVal() {std::cerr << "accessing empty cell values"; throw this;};
//    long getLongVal() {std::cerr << "accessing empty cell values"; throw this;};
//    std::string getStringVal() {std::cerr << "accessing empty cell values"; throw this;};
//    std::string toString(int precision) {return "-";}
//};


class Row {
public:
    std::vector<std::shared_ptr<CellInterface>> cells;
    Row(unsigned long size) : cells() {
        for (int i=0;i<size;i++) {
            cells.emplace_back(std::shared_ptr<CellInterface>(nullptr));
        }
    }

    std::shared_ptr<CellInterface>& operator[] (unsigned long i) {
        return cells.at(i);
    }

//    void addCell(int location, std::shared_ptr<CellInterface> cell) {
//        cells[location] = cell;
//    }

    inline std::vector<std::shared_ptr<CellInterface>> getCells() {
        return cells;
    }
};
