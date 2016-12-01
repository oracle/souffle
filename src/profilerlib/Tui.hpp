
#include <iostream>
#include <string>
#include <vector>

#include "OutputProcessor.hpp"
#include "Reader.hpp"

class Tui {
private:
    OutputProcessor out;
    bool loaded;
	std::string f_name;
	bool alive = false;
	int sort_col = 0;
	int precision = -1;
    Table rel_table_state;
    Table rul_table_state;
    int sortDir = 1;
    static const int SCREEN_WIDTH = 64; //characters

public:
	Tui(std::string filename, bool live);

    void runCommand(std::vector<std::string> c);
    void runProf();
    void loadMenu();
    void quit();
    void save(std::string save_name);
    void load(std::string method, std::string load_file);
    void help();
    void top();
    void rel(std::string c);
    void rul(std::string c);
    void id(std::string col);
    void relRul(std::string str);
    void verRul(std::string str);
    void iterRel(std::string c, std::string col);
    void iterTul(std::string c, std::string col);
    void verGraph(std::string c, std::string col);
    void graphD(std::vector<double> list);
    void graphL(std::vector<long> list);
};