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
        while (getline(file, str)) {
//            std::cout << "\n" << str ;
            if (!str.empty() && str.at(0) == '@') {
                if (str.compare("@start-debug") == 0) {
                    continue;
                }
//                std::vector<std::string> part;
//                if (str.find("\'") != std::string::npos ||
//                    str.find("\"") != std::string::npos) {
//                    part = replace(str);
//                } else {
//                    part = split(str, "\\s*;\\s*");
//
//                }
                // one regex split, instead of unnecessary function
                // From: http://markmintoff.com/2013/03/regex-split-by-comma-not-surrounded-by-quotes/
                std::vector<std::string> part = split(str.substr(1), "\\s*;(?=(?:[^\"]*\"[^\"]*\")*[^\"]*$)\\s*");

//                std::string teststr = "";
//                teststr += "\nLINE SPLIT: [(";
//                for (auto &i : part) {
//                    teststr+= i + "),(";
//                }
//                std::cout << teststr.substr(0, teststr.size()-2) + "]\n";

                process(part);
            }
        }
        file.close();
        loaded = true;
    }
}

void Reader::process(const std::vector<std::string>& data) {

    if (data[0].compare("runtime") == 0) {
        runtime = stod(data[1]);
    } else {
        //insert into the map if it does not exist already
        if (relation_map.find(data[1]) == relation_map.end()) {
            relation_map.emplace(data[1], std::make_shared<Relation>(Relation(data[1], createId())));
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
        // assert(rul);
        _rul->setNum_tuples(std::stol(data[4]) - prev_num_tuples);
        rel->setPrev_num_tuples(std::stol(data[4]));
    }
}



std::vector<std::string> Reader::split(std::string str, std::string split_reg) {
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

//std::vector<std::string> Reader::replace(std::string str) {
//    // ignore semicolons from inside strings, then cut where remaining semicolons are
//    // then insert semicolons back into strings
//    bool ignore = false;
//    std::string temp1 = "";
//    if (str.find("\'")!=std::string::npos) {
//        for (auto &c : str) {
//            if (c == '\'') {
//                ignore = !ignore;
//            }
//            if (ignore && c==';') {
//                temp1 += '\b';
//            } else {
//                temp1 += c;
//            }
//        }
//    } else {
//        temp1 = str;
//    }
//    ignore = false;
//    std::string temp2 = "";
//    if (temp1.find("\"")!=std::string::npos) {
//        for (auto &c : temp1) {
//            if (c == '\"') {
//                ignore = !ignore;
//            }
//            if (ignore && c==';') {
//                temp2 += '\t';
//            } else {
//                temp2 += c;
//            }
//        }
//    } else
//        temp2 = temp1;
//    }
//    temp2 = temp2.substr(1);
//    std::vector<std::string> part = split(str.substr(1), "\\s*;\\s*");
//    // go through each string and put semicolons back in where needed
//    for (auto& i : part) {
//        for (auto& c : i) {
//            if (c == '\b' || c == '\t') {
//                c = ';';
//            }
//        }
//    }
//    return part;
//}

std::string Reader::createId() {
    return "R" + std::to_string(++rel_id);
}