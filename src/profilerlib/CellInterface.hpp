//
// Created by Dominic Romanowski on 30/11/16.
//

#pragma once

class CellInterface {
public:
    virtual std::string toString(int precision) = 0;
    virtual double getDoubVal() = 0;
    virtual long getLongVal() = 0;
    virtual std::string getStringVal() = 0;
};
