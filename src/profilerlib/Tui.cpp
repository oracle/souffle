

#include "Tui.hpp"


Tui::Tui(std::string filename, bool live) {
    out = OutputProcessor();
    std::shared_ptr<ProgramRun>& run = out.getProgramRun();
//		std::cout << filename << std::endl;
    Reader read = Reader(filename, run, false, live);
    read.readFile();

    std::unordered_map<std::string, std::shared_ptr<Relation>>& rel_map = run->getRelation_map();

    std::cout << "Relation Table:\n\n";

    std::cout << "{\n";
    for (auto it = rel_map.begin(); it != rel_map.end(); ++it) {
        std::cout <<  "\"" << it->first << "\":" << it->second->toString() << ",\n";
    }
    std::cout << "}\n";

    rul_table_state = out.getRulTable();
    rel_table_state = out.getRelTable();

    std::cout << "Rul Table:\n\n";
    for (auto& row : rul_table_state.getRows()) {
        for (auto & cell : row->getCells()) {
            if (cell != nullptr) {
                std::cout << "|" << cell->toString(precision) << "|";
            } else {
                std::cout << "|-|";
            }
        }
        std::cout << "\n";
    }

    std::cout << "Rel Table:\n\n";
    for (auto& row : rel_table_state.getRows()) {
        for (auto & cell : row->getCells()) {
            if (cell != nullptr) {
                std::cout << "|" << cell->toString(precision) << "|";
            } else {
                std::cout << "|-|";
            }
        }
        std::cout << "\n";
    }
}

void Tui::runCommand(std::vector<std::string> c) {
    if (!loaded) {
        std::cout << "Error: File cannot be loaded\n";
        return;
    }
}

void Tui::runProf() {

}

void Tui::loadMenu() {

}

void Tui::quit() {

}

void Tui::save(std::string save_name) {

}

void Tui::load(std::string method, std::string load_file) {

}

void Tui::help() {

}

void Tui::top() {

}

void Tui::rel(std::string c) {

}

void Tui::rul(std::string c) {

}

void Tui::id(std::string col) {

}

void Tui::relRul(std::string str) {

}

void Tui::verRul(std::string str) {

}

void Tui::iterRel(std::string c, std::string col) {

}

void Tui::iterTul(std::string c, std::string col) {

}

void Tui::verGraph(std::string c, std::string col) {

}

void Tui::graphD(std::vector<double> list) {

}

void Tui::graphL(std::vector<long> list) {
//    long max = 0;
//    for (auto& l : list) {
//        if (l > max) {
//            max = l;
//        }
//    }
//    int i=0;
//    for (auto& l : list) {
//        int len = (int) (l/max*SCREEN_WIDTH);
//        std::cout << out.formatNum(precision, l);
//
//    }
}
