#pragma once

#include <iostream>
#include <fstream>

#define INFO 4
#define MEM 3
#define WARN 2
#define ERR 1

#define PREI <<__TIME__<<" "<<__FILE__<<":"<<__LINE__<<" "
#define PRE <<__TIME__<<" "<<__FILE__<<":"<<__LINE__<<" "<<"    "

#define ENTERJNI(x) PREI << "JNI -- "<< x <<"{\n"
#define LEAVEJNI PREI << "}\n"

#define ENTERCPP(x) PRE << "CPP -- "<< x <<"{\n"
#define LEAVECPP PRE << "}\n"

#define LOG_FILTER 4

namespace souffle {

class LOG
{
public:
    LOG(int level) 
    : m_output(level <= LOG_FILTER) {}

    template<typename T>
    LOG& operator<<(T t)
    {
        if(m_output){
          std::ofstream ofs;
          ofs.open("./LOG.txt", std::ofstream::out | std::ofstream::app);
          ofs << t;
          ofs.close();
          return *this;
        }
        else
          return *this;
    }
private:
    bool m_output;
};

}
