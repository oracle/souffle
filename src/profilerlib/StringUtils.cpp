//
// Created by Dominic Romanowski on 30/11/16.
//


#include "StringUtils.hpp"


std::string Tools::formatNum(int precision, long amount) {
    // assumes number is < 999*10^12
    if (amount == 0) {
        return "0";
    }


    std::string result;

    for (int i=0;i<abbreviations.size();++i) {
        if (amount>std::pow(1000,i+2)) {
            continue;
        }

        if (i==0) {
            return std::to_string(amount);
        }

        double r = amount/std::pow(1000,i+1);
        result = std::to_string(r);

        if (r >= 100) {// 1000 > result >= 100

            //not sure why anyone would do such a thing.
            switch (precision) {
                case -1: break;
                case 0: result = "0"; break;// I guess 0 would be valid?
                case 1: result = result.substr(0,1)+"00"; break;
                case 2: result = result.substr(0,2)+"0"; break;
                case 3: result = result.substr(0,3); break;
                default: result = result.substr(0,precision+1);
            }
        } else if (r >= 10) {// 100 > result >= 10
            switch (precision) {
                case -1: break;
                case 0: result = "0"; break;// I guess 0 precision would be valid?
                case 1: result = result.substr(0,1)+"0"; break;
                case 2: result = result.substr(0,2); break;
                default: result = result.substr(0,precision+1);
            }
        } else { // 10 > result > 0
            switch (precision) {
                case -1: break;
                case 0: result = "0"; break;// I guess 0 precision would be valid?
                case 1: result = result.substr(0,1); break;
                default: result = result.substr(0,precision+1);
            }
        }
        result += abbreviations.at(i);
        return result;
    }
    return NULL;
}


std::string Tools::formatTime(double number) {
    long sec = std::lround(number);
    if (sec >= 100) {
        long min = (long)std::floor(sec/60);
        if (min >= 100) {
            long hours = (long)std::floor(min/60);
            if (hours >= 100) {
                long days = (long)std::floor(hours/24);
                return std::to_string(days) + "D";
            }
            return std::to_string(hours) + "h";
        }
        if (min < 10) {
            // temp should always be 1 digit long
            long temp = (long)std::floor((sec-(min*60.0))*10.0/6.0); // x*10/6 instead of x/60*100
            return std::to_string(min) + "." + std::to_string(temp).substr(0,1)+"m";
        }
        return std::to_string(min) + "m";
    } else if (sec >= 10) {
        return std::to_string(sec)+"s";
    } else if (sec >= 1) {
        std::string temp = std::to_string(std::round(number*100));
        return temp.substr(0,1)+"."+temp.substr(1,2);
    } else if (number >= 0.100) {
        std::string temp = std::to_string(std::round(number*1000));
        return "."+temp.substr(0,3);
    } else if (number > 0.010) {
        std::string temp = std::to_string(std::round(number*1000));
        return ".0"+temp.substr(0,2);
    } else if (number > 0.001) {
        std::string temp = std::to_string(std::round(number*1000));
        return ".00"+temp.substr(0,1);
    }
    return ".000";
}

std::vector<std::vector<std::string>> Tools::formatTable(Table table, int precision) {
    std::vector<std::vector<std::string>> result;
    for (auto& row : table.getRows()) {
        std::vector<std::string> result_row;
        for (auto & cell : row->getCells()) {
            if (cell != nullptr) {
                result_row.push_back(cell->toString(precision));
            } else {
                result_row.push_back("-");
            }
        }
        result.push_back(result_row);
    }
    return result;

}


std::vector<std::string> Tools::split(std::string str, std::string split_reg) {
    std::vector<std::string> elems;

    std::regex rgx(split_reg);
    std::sregex_token_iterator iter(str.begin(),
                                    str.end(),
                                    rgx,
                                    -1);

    std::sregex_token_iterator end;
    for ( ; iter != end; ++iter) {
        elems.push_back(*iter);
    }

    return elems;

}