//
// Created by Dominic Romanowski on 26/11/16.
//

#include "Reader.hpp"



void Reader::readFile() {
    if (isLive()) {
        std::cerr << "not implemented";
        throw std::exception();
    } else {
        if (!file.is_open()) {
            throw std::exception();
        }

        std::string str;
        int lineno = 0;
        while (getline(file, str)) {
            lineno++;
            if (!str.empty() && str.at(0) == '@') {
                if (str.compare("@start-debug") == 0) {
                    continue;
                }
                // regex to split on ';' where ';' isn't in a string eg. "asdf;asdf", 'asdf;asdf',
                // TODO: both ' and "
                std::vector<std::string> part = Tools::split(str.substr(1), "\\s*;(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)\\s*");
                process(part);
            }
        }
        file.close();
        loaded = true;
    }
}

void Reader::save(std::string f_name) {


    std::string workingdir = Tools::getworkingdir();
    if (workingdir.size()==0) {
        std::cerr << "could not get working directory\n" << std::endl;
        throw 1;
    }

    DIR *dir;
    struct dirent *ent;
    bool exists = false;

    if ((dir = opendir((workingdir+std::string("/old_runs")).c_str())) != NULL) {
        exists = true;
        closedir(dir);
    }

    if (!exists) {
        std::string sPath = workingdir+std::string("/old_runs");
        mode_t nMode = 0733; // UNIX style permissions
        int nError = 0;
        #if defined(WINDOWS)
                nError = _mkdir(sPath.c_str()); // can be used on Windows
        #else
                nError = mkdir(sPath.c_str(),nMode); // can be used on non-Windows
        #endif
        if (nError != 0) {
            std::cerr << "directory ./old_runs/ failed to be created.";
            exit(2);
        }
    }

    std::string new_file = workingdir+std::string("/old_runs/");
    if (Tools::file_exists(new_file)) {
        int i = 1;
        while (Tools::file_exists(new_file + std::to_string(i))){
            i++;
        }

        new_file = new_file+std::to_string(i);
    }

    std::ofstream fout(new_file);

    if (!file.is_open()) {
        throw std::exception();
    }

    time_t t = time(0);   // get time now
    struct tm * now = localtime( & t );
    fout << this->file_loc << " created on "
            << (now->tm_year + 1900) << '.'
         << (now->tm_mon + 1) << '.'
         <<  now->tm_mday << " at "
            << now->tm_hour << ":" << now->tm_min << ":" << now->tm_sec
         << std::endl;

    std::string str;
    while (getline(file, str)) {
        fout << str << "\n";
    }
}

void Reader::process(const std::vector<std::string>& data) {
    if (data[0].compare("runtime") == 0) {
        runtime = stod(data[1]);
    } else {
        //insert into the map if it does not exist already
        if (relation_map.find(data[1]) == relation_map.end()) {
            relation_map.emplace(data[1], std::make_shared<Relation>(Relation(data[1], createId())));
        } else {
        }
        std::shared_ptr<Relation> _rel = relation_map[data[1]];

        // find non-recursive first, since they both share text recursive
        if (data[0].find("nonrecursive") != std::string::npos) {
            if (data[0].at(0) == 't' && data[0].find("relation") != std::string::npos) {
                _rel->setRuntime(std::stod(data[3]));
                _rel->setLocator(data[2]);
            } else if (data[0].at(0) == 'n' && data[0].find("relation") != std::string::npos) {
                _rel->setNum_tuples(std::stol(data[3]));
            } else if (data[0].find("rule") != std::string::npos) {
                addRule(_rel, data);
            }
        } else if (data[0].find("recursive") != std::string::npos) {
            addIteration(_rel, data);
        }
    }

    run->SetRuntime(this->runtime);
    run->setRelation_map(this->relation_map);
}


void Reader::addIteration(std::shared_ptr<Relation> rel, std::vector<std::string> data) {

    bool ready = rel->isReady();
    std::vector<std::shared_ptr<Iteration>>& iterations = rel->getIterations();
    std::string locator = rel->getLocator();


    // add an iteration if we require one
    if (ready || iterations.empty()) {
        iterations.push_back(std::make_shared<Iteration>(Iteration()));
        rel->setReady(false);
    }

    std::shared_ptr<Iteration>& iter = iterations.back();

    if (data[0].find("rule") != std::string::npos) {
        std::string temp = rel->createRecID(data[4]);
        iter->addRule(data, temp);
    } else if (data[0].at(0) == 't' && data[0].find("relation") != std::string::npos) {
        iter->setRuntime(std::stod(data[3]));
        iter->setLocator(data[2]);
        rel->setLocator(data[2]);
    } else if (data[0].at(0) == 'n' && data[0].find("relation") != std::string::npos) {
        iter->setNum_tuples(std::stol(data[3]));
    } else if (data[0].at(0) == 'c' && data[0].find("relation") != std::string::npos) {
        iter->setCopy_time(std::stod(data[3]));
        rel->setReady(true);
    }
}


void Reader::addRule(std::shared_ptr<Relation> rel, std::vector<std::string> data) {

    std::unordered_map<std::string,std::shared_ptr<Rule>>& ruleMap = rel->getRuleMap();

    long prev_num_tuples = rel->getPrev_num_tuples();
    if (ruleMap.find(data[3]) == ruleMap.end()) {
        ruleMap.emplace(data[3],std::make_shared<Rule>(Rule(data[3], rel->createID())));
    }

    std::shared_ptr<Rule> _rul = ruleMap[data[3]];


    if (data[0].at(0) == 't') {
        _rul->setRuntime(std::stod(data[4]));
        _rul->setLocator(data[2]);
    } else if (data[0].at(0) == 'n') {
        assert(_rul != nullptr);
        _rul->setNum_tuples(std::stol(data[4]) - prev_num_tuples);
        rel->setPrev_num_tuples(std::stol(data[4]));
    }
}


std::string Reader::createId() {
    return "R" + std::to_string(++rel_id);
}