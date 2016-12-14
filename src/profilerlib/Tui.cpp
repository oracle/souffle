

#include "Tui.hpp"
#include "DataComparator.hpp"
#include <dirent.h>

Tui::Tui(std::string filename, bool live) {

    this->f_name = filename;

    out = OutputProcessor();
    std::shared_ptr<ProgramRun>& run = out.getProgramRun();

    Reader read = Reader(filename, run, false, live);
    read.readFile();
    this->loaded = read.isLoaded();

    rul_table_state = out.getRulTable();
    rel_table_state = out.getRelTable();


}

void Tui::runCommand(std::vector<std::string> c) {
    if (!loaded) {
        std::cout << "Error: File cannot be loaded\n";
        return;
    }
    if (c[0].compare("top")==0) {
        top();
    } else if (c[0].compare("rel")==0) {
        if (c.size() == 2) {
            relRul(c[1]);
        } else if (c.size() == 1) {
            rel(c[0]);
        } else {
            std::cout << "Invalid parameters to rel command.\n";
            help();
        }
    } else if (c[0].compare("rul")==0) {
        if (c.size() > 1) {
            if (c.size() == 3 && c[1].compare("id")==0) {
                std::printf("%7s%2s%-25s\n\n", "ID", "", "NAME");
                id(c[2]);
            } else if (c.size() == 2 && c[1].compare("id") == 0) {
                id("0");
            } else if (c.size() == 2) {
                verRul(c[1]);
            } else {
                std::cout << "Invalid parameters to rul command.\n";
                help();
            }
        } else {
            rul(c[0]);
        }
    } else if (c[0].compare("graph")==0) {
        if (c.size() == 3 && c[1].find(".")==std::string::npos) {
            iterRel(c[1], c[2]);
        } else if (c.size() == 3 && c[1].at(0)=='C') {
            iterRul(c[1],c[2]);
        } else if (c.size() == 4 && c[1].compare("ver")==0 &&
                c[2].at(0)=='C') {
            verGraph(c[2],c[3]);
        } else {
            std::cout << "Invalid parameters to graph command.\n";
            help();
        }
    } else if (c[0].compare("help")==0) {
        help();
    } else {
        std::cout <<  "Unknown command. Please select from the following commands:\n";
        help();
    }
}

void Tui::runProf() {


    if (!loaded && !f_name.empty()) {
        std::cout << "Error: File cannot be loaded\n";
        return;
    }
    if (loaded) {
        std::cout << "SouffleProf v2.1.8\n";
        top();
    }
    while (true) {
        if (!loaded) {
            loadMenu();
            if (!f_name.empty()) {
                std::cout << "Error loading file.\n";
            }
        }
        std::string input;
        std::cout << "> ";
        getline (std::cin, input);
        //if (input.compare("")==0) {
        //    std::cout << "Error reading command.\n";
        //    return;
        //}

        std::vector<std::string> c = Tools::split(input, "\\s+");

        if (c[0].compare("q")==0 || c[0].compare("quit")==0) {
            quit();
            break;
        } else if (c[0].compare("load")==0 || c[0].compare("open")==0) {
            if (c.size() == 2) {
                load(c[0], c[1]);
            } else {
                loadMenu();
            }
        } else if (c[0].compare("save")==0) {
            if (c.size() == 1) {
                std::cout << "Enter file name to save.\n";
            } else if (c.size() == 2) {
                save(c[1]);
            }
        } else if (c[0].compare("sort")==0) {
            if (c.size() == 2 && std::stoi(c[1]) < 7) {
                sort_col = std::stoi(c[1]);
            } else {
                std::cout << "Invalid column, please select a number between 0 and 6.\n";
            }
        } else {
            runCommand(c);
        }
    }
}

void Tui::loadMenu() {
    std::cout << "Please 'load' a file or 'open' from Previous Runs.\n";
    std::cout << "Previous Runs:\n";

    // http://stackoverflow.com/a/612176
    // TODO: check cross-platform capability
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir("./old_runs")) != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            // if the file doesnt exist in the working directory, it is in old_runs (to remove . and ..)
            if (!Tools::file_exists(ent->d_name)) {
                printf("- %s\n", ent->d_name);
            }
        }
        closedir(dir);
    }
//    else {
//        do nothing
//    }
}

void Tui::quit() {
    if (alive && loaded) {
        std::cerr << "Liver reader not implemented\n";
        // live_reader.stopRead();
    }
}

void Tui::save(std::string save_name) {
    if (loaded) {
        std::shared_ptr<ProgramRun>& run = out.getProgramRun();
        Reader saver(this->f_name, run, false, false);
        saver.save(save_name);
        std::cout << "Save success.\n";
    } else {
        std::cout << "Save failed.\n";
    }
}

void Tui::load(std::string method, std::string load_file) {
    std::shared_ptr<ProgramRun> new_run = std::make_shared<ProgramRun>(ProgramRun());
    std::string f_name = load_file;
    // if load, should be a valid filepath
    if (method.compare("open") == 0) {
        f_name = Tools::getworkingdir()+"/old_runs/"+load_file;
    }
    Reader loader(f_name, new_run, false, false);
    loader.readFile();
    if (loader.isLoaded()) {
        std::cout << "Load success\n";
        this->loaded = true;
        this->f_name = f_name;
        if (alive) {
            std::cout << "Live reader not implemented\n" << std::endl;
            throw;
        }
        top();
    } else {
        std::cout << "Error: File not found\n";
    }
}

void Tui::help() {
    std::cout << "\nAvailable profiling commands:" << std::endl;
    std::printf("  %-30s%-5s %-10s\n", "rel", "-",
                                    "display relation table.");
    std::printf("  %-30s%-5s %-10s\n", "rel <relation id>",
                                    "-", "display all rules of given relation.");
    std::printf("  %-30s%-5s %-10s\n", "rul", "-",
                                    "display rule table");
    std::printf("  %-30s%-5s %-10s\n", "rul <rule id>", "-",
                                    "display all version of given rule.");
    std::printf("  %-30s%-5s %-10s\n", "rul id", "-",
                                    "display all rules names and ids.");
    std::printf("  %-30s%-5s %-10s\n", "rul id <rule id>",
                                    "-", "display the rule name for the given rule id.");
    std::printf("  %-30s%-5s %-10s\n",
                                    "graph <relation id> <type>", "-",
                                    "graph the relation by type(tot_t/copy_t/tuples).");
    std::printf("  %-30s%-5s %-10s\n",
                                    "graph <rule id> <type>", "-",
                                    "graph the rule (C rules only)  by type(tot_t/tuples).");
    std::printf("  %-30s%-5s %-10s\n",
                                    "graph ver <rule id> <type>", "-",
                                    "graph the rule versions (C rules only) by type(tot_t/tuples).");
    std::printf("  %-30s%-5s %-10s\n", "top", "-",
                                    "display top-level summary of program run.");
    std::printf("  %-30s%-5s %-10s\n", "help", "-",
                                    "print this.");

    std::cout << "\nInteractive mode only commands:" << std::endl;
    std::printf("  %-30s%-5s %-10s\n", "load <filename>",
                                    "-", "load the given new log file.");
    std::printf("  %-30s%-5s %-10s\n", "open", "-",
                                    "list stored program run log files.");
    std::printf("  %-30s%-5s %-10s\n", "open <filename>",
                                    "-", "open the given stored log file.");
    std::printf("  %-30s%-5s %-10s\n", "save <filename>",
                                    "-", "store a log file.");
    std::printf("  %-30s%-5s %-10s\n", "stop", "-",
                                    "stop running live.");
    std::printf("  %-30s%-5s %-10s\n", "sort <col number>",
                                    "-", "sets sorting to be by given column number (0 indexed).");
    std::printf("  %-30s%-5s %-10s\n", "q", "-",
                                    "exit program.");
}

void Tui::top() {
    std::shared_ptr<ProgramRun>& run = out.getProgramRun();
    std::string runtime = run->getRuntime();
    std::cout << "\n Total runtime: " <<  runtime << "\n";


    std::cout << "\n Total number of new tuples: " << run->formatNum(precision, run->getTotNumTuples()) << std::endl;
}

void Tui::rel(std::string c) {
    rel_table_state.sort(sort_col);
    std::cout << " ----- Relation Table -----\n";
    std::printf("%8s%8s%8s%8s%15s%6s%1s%-25s\n\n",
                "TOT_T", "NREC_T", "REC_T", "COPY_T", "TUPLES", "ID", "", "NAME");
    for (auto& row : out.formatTable(rel_table_state, precision)) {
        std::printf("%8s%8s%8s%8s%15s%6s%1s%-5s\n",
                    row[0].c_str(), row[1].c_str(), row[2].c_str(),
                    row[3].c_str(), row[4].c_str(), row[6].c_str(),
                    "", row[5].c_str());
    }
}

void Tui::rul(std::string c) {
    rul_table_state.sort(sort_col);
    std::cout << "  ----- Rule Table -----\n";
    std::printf("%8s%8s%8s%8s%15s    %-5s\n\n", "TOT_T",
                "NREC_T", "REC_T", "COPY_T", "TUPLES", "ID RELATION");
    for (auto& row: out.formatTable(rul_table_state, precision)) {
        std::printf("%8s%8s%8s%8s%15s%8s %-25s\n",
                    row[0].c_str(), row[1].c_str(), row[2].c_str(),
                    row[3].c_str(), row[4].c_str(), row[6].c_str(), row[7].c_str());
    }

}

void Tui::id(std::string col) {
    rul_table_state.sort(6);
    std::vector<std::vector<std::string>> table = out.formatTable(rul_table_state, precision);

    if (col.compare("0") == 0) {
        std::printf("%7s%2s%-25s\n\n", "ID", "", "NAME");
        for (auto& row : table) {
            std::printf("%7s%2s%-25s\n", row[6].c_str(), "", row[5].c_str());
        }
    } else {
        for (auto& row : table) {
            if (row[6].compare(col) == 0) {
                std::printf("%7s%2s%-25s\n", row[6].c_str(), "", row[5].c_str());
            }
        }
    }
}

void Tui::relRul(std::string str) {
    rul_table_state.sort(sort_col);

    std::vector<std::vector<std::string>> rul_table = out.formatTable(rul_table_state, precision);
    std::vector<std::vector<std::string>> rel_table = out.formatTable(rel_table_state, precision);

    std::cout << "  ----- Rules of a Relation -----\n";
    std::printf("%8s%8s%8s%8s%10s%8s %-25s\n\n", "TOT_T",
                "NREC_T", "REC_T", "COPY_T", "TUPLES", "ID", "NAME");
    std::string name = "";
    bool found = false; // workaround to make it the same as java (row[5] seems to have priority)
    for (auto& row : rel_table) {
        if (row[5].compare(str) == 0 ) {
            std::printf("%8s%8s%8s%8s%10s%8s %-25s\n",
                        row[0].c_str(), row[1].c_str(),
                        row[2].c_str(), row[3].c_str(),
                        row[4].c_str(), row[6].c_str(),
                        row[5].c_str());
            name = row[5];
            found = true;
            break;
        }
    }
    if (!found) {
        for (auto& row : rel_table) {
            if (row[6].compare(str) == 0) {
                std::printf("%8s%8s%8s%8s%10s%8s %-25s\n",
                            row[0].c_str(), row[1].c_str(),
                            row[2].c_str(), row[3].c_str(),
                            row[4].c_str(), row[6].c_str(),
                            row[5].c_str());
                name = row[5];
                break;
            }
        }
    }
    std::cout << " ---------------------------------------------------------\n";
    for (auto& row : rul_table) {
        if (row[7].compare(name)==0) {
            std::printf("%8s%8s%8s%8s%10s%8s %-25s\n",
                        row[0].c_str(), row[1].c_str(),
                        row[2].c_str(), row[3].c_str(),
                        row[4].c_str(), row[6].c_str(),
                        row[7].c_str());
        }
    }
    std::string src = "";
    std::shared_ptr<ProgramRun>& run = out.getProgramRun();
    if (run->getRelation(name) != nullptr) {
        src = run->getRelation(name)->getLocator();
    }
    std::cout << "\nSrc locator: " << src << "\n\n";
    for (auto& row : rul_table) {
        if (row[7].compare(name) == 0) {
            std::printf("%7s%2s%-25s\n", row[6].c_str(), "", row[5].c_str());
        }
    }
}

void Tui::verRul(std::string str) {
    if (str.find(".") == std::string::npos) {
        std::cout << "Rule does not exist\n";
        return;
    }
    std::vector<std::string> part = Tools::split(str, "\\.");
    std::string strRel = "R" + part[0].substr(1);

    Table ver_table = out.getVersions(strRel, str);
    ver_table.sort(sort_col);

    rul_table_state.sort(sort_col); // why isnt it sorted in the original java?!?

    std::vector<std::vector<std::string>> rul_table = out.formatTable(rul_table_state, precision);




    std::cout << "  ----- Rule Versions Table -----\n";
    std::printf("%8s%8s%8s%8s%10s%6s   %-5s\n\n", "TOT_T",
                "NREC_T", "REC_T", "COPY_T", "TUPLES", "VER", "ID RELATION");
    bool found = false;
    for (auto& row : rul_table) {
        if (row[6].compare(str) == 0) {
            std::printf("%8s%8s%8s%8s%10s%6s%7s %-25s\n",
                        row[0].c_str(), row[1].c_str(),
                        row[2].c_str(), row[3].c_str(),
                        row[4].c_str(), "",
                        row[6].c_str(), row[7].c_str());
            found = true;
        }
    }
    std::cout << " ---------------------------------------------------------\n";
    for (auto& _row : ver_table.rows) {
        Row row = *_row;

        std::printf("%8s%8s%8s%8s%10s%6s%7s %-25s\n",
                    row[0]->toString(precision).c_str(), row[1]->toString(precision).c_str(),
                    row[2]->toString(precision).c_str(), row[3]->toString(precision).c_str(),
                    row[4]->toString(precision).c_str(), row[8]->toString(precision).c_str(),
                    row[6]->toString(precision).c_str(), row[7]->toString(precision).c_str());

    }
    if (found) {
        if (ver_table.rows.size() > 0) {
            if (ver_table.rows[0]->cells[9]!=nullptr) {
                std::cout << "\nSrc locator: " << (*ver_table.rows[0])[9]->getStringVal() << "\n\n";
            } else {
                std::cout << "\nSrc locator: -\n\n";
            }
        } else if (rul_table.size() > 0) {
            std::cout << "\nSrc locator-: " << rul_table[0][10] << "\n\n";
        }
    }

    for (auto& row : rul_table) {
        if (row[6].compare(str) == 0) {
            std::printf("%7s%2s%-25s\n", row[6].c_str(), "", row[5].c_str());
        }
    }
}


void Tui::iterRel(std::string c, std::string col) {
    std::vector<std::vector<std::string>> table = out.formatTable(rel_table_state, -1);
    std::vector<std::shared_ptr<Iteration>> iter;
    for (auto& row : table) {
        if (row[6].compare(c) == 0) {
            std::printf("%4s%2s%-25s\n\n", row[6].c_str(), "", row[5].c_str());
            std::shared_ptr<ProgramRun>& run = out.getProgramRun();
            iter = run->getRelation_map()[row[5]]->getIterations();
            if (col.compare("tot_t") == 0) {
                std::vector<double> list;
                for (auto& i : iter) {
                    list.emplace_back(i->getRuntime());
                }
                std::printf("%4s   %-6s\n\n", "NO", "RUNTIME");
                graphD(list);
            } else if (col.compare("copy_t") == 0) {
                std::vector<double> list;
                for (auto& i : iter) {
                    list.emplace_back(i->getCopy_time());
                }
                std::printf("%4s   %-6s\n\n", "NO", "COPYTIME");
                graphD(list);
            } else if (col.compare("tuples") == 0) {
                std::vector<long> list;
                for (auto &i : iter) {
                    list.emplace_back(i->getNum_tuples());
                }
                std::printf("%4s   %-6s\n\n", "NO", "TUPLES");
                graphL(list);
            }
            return;
        }
    }
    for (auto& row : table) {
        if (row[5].compare(c) == 0) {
            std::printf("%4s%2s%-25s\n\n", row[6].c_str(), "", row[5].c_str());
            std::shared_ptr<ProgramRun>& run = out.getProgramRun();
            iter = run->getRelation_map()[row[5]]->getIterations();
            if (col.compare("tot_t") == 0) {
                std::vector<double> list;
                for (auto& i : iter) {
                    list.emplace_back(i->getRuntime());
                }
                std::printf("%4s   %-6s\n\n", "NO", "RUNTIME");
                graphD(list);
            } else if (col.compare("copy_t") == 0) {
                std::vector<double> list;
                for (auto& i : iter) {
                    list.emplace_back(i->getCopy_time());
                }
                std::printf("%4s   %-6s\n\n", "NO", "COPYTIME");
                graphD(list);
            } else if (col.compare("tuples") == 0) {
                std::vector<long> list;
                for (auto &i : iter) {
                    list.emplace_back(i->getNum_tuples());
                }
                std::printf("%4s   %-6s\n\n", "NO", "TUPLES");
                graphL(list);
            }
            return;
        }
    }
}

void Tui::iterRul(std::string c, std::string col) {
    std::vector<std::vector<std::string>> table = out.formatTable(rul_table_state, precision);
    std::vector<std::shared_ptr<Iteration>> iter;
    for (auto& row : table) {
        if (row[6].compare(c) == 0) {
            std::printf("%6s%2s%-25s\n\n", row[6].c_str(), "", row[5].c_str());
            std::shared_ptr<ProgramRun>& run = out.getProgramRun();
            iter = run->getRelation_map()[row[7]]->getIterations();
            if (col.compare("tot_t") == 0) {
                std::vector<double> list;
                for (auto& i : iter) {
                    bool add = false;
                    double tot_time = 0.0;
                    for (auto& rul : i->getRul_rec()) {
                        if (rul.second->getId().compare(c) == 0) {
                            tot_time += rul.second->getRuntime();
                            add = true;
                        }
                    }
                    if (add) {
                        list.emplace_back(tot_time);
                    }
                }
                std::printf("%4s   %-6s\n\n", "NO", "RUNTIME");
                graphD(list);
            } else if (col.compare("tuples")==0) {
                std::vector<long> list;
                for (auto& i : iter) {
                    bool add = false;
                    long tot_num = 0L;
                    for (auto &rul : i->getRul_rec()) {
                        if (rul.second->getId().compare(c) == 0) {
                            tot_num += rul.second->getNum_tuples();
                            add = true;
                        }
                    }
                    if (add) {
                        list.emplace_back(tot_num);
                    }
                }
                std::printf("%4s   %-6s\n\n", "NO", "TUPLES");
                graphL(list);
            }
            break;
        }
    }
}

void Tui::verGraph(std::string c, std::string col) {
    if (c.find('.')==std::string::npos) {
        std::cout << "Rule does not exist";
        return;
    }

    std::vector<std::string> part = Tools::split(c, "\\.");
    std::string strRel = "R"+part[0].substr(1);

    Table ver_table = out.getVersions(strRel, c);
    // std::printf("%6s%2s%-25s\n\n", ver_table[0][6], "", ver_table[0][5])));
    std::printf("%6s%2s%-25s\n\n", (*ver_table.rows[0])[6]->getStringVal().c_str(), "", (*ver_table.rows[0])[5]->getStringVal().c_str());
    if (col.compare("tot_t") == 0) {
        std::vector<double> list;
        for (auto& row : ver_table.rows) {
            list.emplace_back((*row)[0]->getDoubVal());
        }
        std::printf("%4s   %-6s\n\n", "NO", "RUNTIME");
        graphD(list);
    } else if (col.compare("copy_t") == 0) {
        std::vector<double> list;
        for (auto& row : ver_table.rows) {
            list.emplace_back((*row)[3]->getDoubVal());
        }
        std::printf("%4s   %-6s\n\n", "NO", "COPYTIME");
        graphD(list);
    } else if (col.compare("tuples") == 0) {
        std::vector<long> list;
        for (auto& row : ver_table.rows) {
            list.emplace_back((*row)[4]->getLongVal());
        }
        std::printf("%4s   %-6s\n\n", "NO", "TUPLES");
        graphL(list);
    }

}

void Tui::graphD(std::vector<double> list) {
    double max = 0;
    for (auto& d : list) {
        if (d > max) {
            max = d;
        }
    }

    std::sort(list.begin(), list.end());
    std::reverse(list.begin(), list.end());
    int i=0;
    for (auto& d : list) {
        int len = (int) (67*(d/max));
        // TODO: %4d %10.8f
        std::string bar = "";
        for (int j=0; j<len;j++) {
            bar += "*";
        }

        if (isnan(d)) {
            std::printf("%4d        NaN | %s\n", i++, bar.c_str());
        } else {
            std::printf("%4d %10.8f | %s\n", i++, d, bar.c_str());
        }

    }
}

void Tui::graphL(std::vector<long> list) {
    long max = 0;
    for (auto& l : list) {
        if (l > max) {
            max = l;
        }
    }
    std::sort(list.begin(), list.end());
    std::reverse(list.begin(), list.end());
    int i=0;
    for (auto& l : list) {
        int len = (int) (64*((double)l/(double)max));
        std::string bar = "";
        for (int j=0; j<len;j++) {
            bar += "*";
        }

        std::printf("%4d %8s | %s\n", i++, out.formatNum(precision, l).c_str(), bar.c_str());

    }
}


bool Tui::string_sort(std::vector<std::string> a, std::vector<std::string> b) {
    //std::cerr << a->getCells()[0]->getDoubVal() << "\n";
    return a[0] > b[0];
}