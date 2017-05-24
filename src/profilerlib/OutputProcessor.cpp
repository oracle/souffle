/*
* Souffle - A Datalog Compiler
* Copyright (c) 2016, The Souffle Developers. All rights reserved
* Licensed under the Universal Permissive License v 1.0 as shown at:
* - https://opensource.org/licenses/UPL
* - <souffle root>/licenses/SOUFFLE-UPL.txt
*/

#include "OutputProcessor.hpp"

/*
 * rel table :
 * ROW[0] = TOT_T
 * ROW[1] = NREC_T
 * ROW[2] = REC_T
 * ROW[3] = COPY_T
 * ROW[4] = TUPLES
 * ROW[5] = REL NAME
 * ROW[6] = ID
 * ROW[7] = SRC
 * ROW[8] = PERFOR
 *
 */
Table OutputProcessor::getRelTable() {
    std::unordered_map<std::string, std::shared_ptr<Relation>>& relation_map = programRun->getRelation_map();
    Table table;
    for (auto& rel : relation_map) {
        std::shared_ptr<Relation> r = rel.second;
        Row row(9);
        double total_time = r->getNonRecTime() + r->getRecTime() + r->getCopyTime();
        row[0] = std::shared_ptr<CellInterface>(new Cell<double>(total_time));
        row[1] = std::shared_ptr<CellInterface>(new Cell<double>(r->getNonRecTime()));
        row[2] = std::shared_ptr<CellInterface>(new Cell<double>(r->getRecTime()));
        row[3] = std::shared_ptr<CellInterface>(new Cell<double>(r->getCopyTime()));
        row[4] = std::shared_ptr<CellInterface>(new Cell<long>(r->getNum_tuplesRel()));
        row[5] = std::shared_ptr<CellInterface>(new Cell<std::string>(r->getName()));
        row[6] = std::shared_ptr<CellInterface>(new Cell<std::string>(r->getId()));
        row[7] = std::shared_ptr<CellInterface>(new Cell<std::string>(r->getLocator()));
        if (total_time != 0.0) {
            row[8] = std::shared_ptr<CellInterface>(new Cell<double>(r->getNum_tuplesRel() / total_time));
        } else {
            row[8] = std::shared_ptr<CellInterface>(new Cell<double>(r->getNum_tuplesRel() / 1.0));
        }

        table.addRow(std::make_shared<Row>(row));
    }
    return table;
}
/*
 * rul table :
 * ROW[0] = TOT_T
 * ROW[1] = NREC_T
 * ROW[2] = REC_T
 * ROW[3] = COPY_T
 * ROW[4] = TUPLES
 * ROW[5] = RUL NAME
 * ROW[6] = ID
 * ROW[7] = SRC
 * ROW[8] = PERFOR
 * ROW[9] = VER
 * ROW[10]= REL_NAME
 */
Table OutputProcessor::getRulTable() {
    std::unordered_map<std::string, std::shared_ptr<Relation>>& relation_map = programRun->getRelation_map();
    std::unordered_map<std::string, std::shared_ptr<Row>> rule_map;

    double tot_rec_tup = programRun->getTotNumRecTuples();
    double tot_copy_time = programRun->getTotCopyTime();

    for (auto& rel : relation_map) {
        for (auto& _rul : rel.second->getRuleMap()) {
            Row row(11);
            std::shared_ptr<Rule> rul = _rul.second;
            row[1] = std::shared_ptr<CellInterface>(new Cell<double>(rul->getRuntime()));
            row[2] = std::shared_ptr<CellInterface>(new Cell<double>(0.0));
            row[3] = std::shared_ptr<CellInterface>(new Cell<double>(0.0));
            row[4] = std::shared_ptr<CellInterface>(new Cell<long>(rul->getNum_tuples()));
            row[5] = std::shared_ptr<CellInterface>(new Cell<std::string>(rul->getName()));
            row[6] = std::shared_ptr<CellInterface>(new Cell<std::string>(rul->getId()));
            row[7] = std::shared_ptr<CellInterface>(new Cell<std::string>(rel.second->getName()));
            row[8] = std::shared_ptr<CellInterface>(new Cell<long>(0));
            row[10] = std::shared_ptr<CellInterface>(new Cell<std::string>(rul->getLocator()));

            rule_map.emplace(rul->getName(), std::make_shared<Row>(row));
        }
        for (auto& iter : rel.second->getIterations()) {
            for (auto& _rul : iter->getRul_rec()) {
                std::shared_ptr<Rule> rul = _rul.second;
                if (rule_map.find(rul->getName()) != rule_map.end()) {
                    std::shared_ptr<Row> _row = rule_map[rul->getName()];
                    Row row = *_row;
                    row[2] = std::shared_ptr<CellInterface>(
                            new Cell<double>(row[2]->getDoubVal() + rul->getRuntime()));
                    row[4] = std::shared_ptr<CellInterface>(
                            new Cell<long>(row[4]->getLongVal() + rul->getNum_tuples()));
                    row[0] = std::shared_ptr<CellInterface>(new Cell<double>(rul->getRuntime()));
                    rule_map[rul->getName()] = std::make_shared<Row>(row);
                } else {
                    Row row(11);
                    row[1] = std::shared_ptr<CellInterface>(new Cell<double>(0.0));
                    row[2] = std::shared_ptr<CellInterface>(new Cell<double>(rul->getRuntime()));
                    row[3] = std::shared_ptr<CellInterface>(new Cell<double>(0.0));
                    row[4] = std::shared_ptr<CellInterface>(new Cell<long>(rul->getNum_tuples()));
                    row[5] = std::shared_ptr<CellInterface>(new Cell<std::string>(rul->getName()));
                    row[6] = std::shared_ptr<CellInterface>(new Cell<std::string>(rul->getId()));
                    row[7] = std::shared_ptr<CellInterface>(new Cell<std::string>(rel.second->getName()));
                    row[8] = std::shared_ptr<CellInterface>(new Cell<long>(rul->getVersion()));
                    row[0] = std::shared_ptr<CellInterface>(new Cell<double>(rul->getRuntime()));
                    rule_map[rul->getName()] = std::make_shared<Row>(row);
                }
            }
        }
        for (auto& _row : rule_map) {
            std::shared_ptr<Row> row = _row.second;
            Row t = *row;
            if (t[6]->getStringVal().at(0) == 'C') {
                double rec_tup = (double)(t[4]->getLongVal());
                t[3] = std::shared_ptr<CellInterface>(
                        new Cell<double>(rec_tup * tot_copy_time / tot_rec_tup));
            }
            double val = t[1]->getDoubVal() + t[2]->getDoubVal() + t[3]->getDoubVal();

            t[0] = std::shared_ptr<CellInterface>(new Cell<double>(val));

            if (t[0]->getDoubVal() != 0.0) {
                t[9] = std::shared_ptr<CellInterface>(
                        new Cell<double>(t[4]->getLongVal() / t[0]->getDoubVal()));
            } else {
                t[9] = std::shared_ptr<CellInterface>(new Cell<double>(t[4]->getLongVal() / 1.0));
            }
            _row.second = std::make_shared<Row>(t);
        }
    }

    Table table;
    for (auto& _row : rule_map) {
        table.addRow(_row.second);
    }
    return table;
}

/*
 * ver table :
 * ROW[0] = TOT_T
 * ROW[1] = NREC_T
 * ROW[2] = REC_T
 * ROW[3] = COPY_T
 * ROW[4] = TUPLES
 * ROW[5] = RUL NAME
 * ROW[6] = ID
 * ROW[7] = SRC
 * ROW[8] = PERFOR
 * ROW[9] = VER
 * ROW[10]= REL_NAME
 */
Table OutputProcessor::getVersions(std::string strRel, std::string strRul) {
    std::unordered_map<std::string, std::shared_ptr<Relation>>& relation_map = programRun->getRelation_map();
    std::unordered_map<std::string, std::shared_ptr<Row>> rule_map;

    double tot_rec_tup = programRun->getTotNumRecTuples();
    double tot_copy_time = programRun->getTotCopyTime();

    for (auto& _rel : relation_map) {
        std::shared_ptr<Relation> rel = _rel.second;
        if (rel->getId().compare(strRel) == 0) {
            for (auto& iter : rel->getIterations()) {
                for (auto& _rul : iter->getRul_rec()) {
                    std::shared_ptr<Rule> rul = _rul.second;
                    if (rul->getId().compare(strRul) == 0) {
                        std::string strTemp =
                                rul->getName() + rul->getLocator() + std::to_string(rul->getVersion());

                        if (rule_map.find(strTemp) != rule_map.end()) {
                            std::shared_ptr<Row> _row = rule_map[strTemp];
                            Row row = *_row;
                            row[2] = std::shared_ptr<CellInterface>(
                                    new Cell<double>(row[2]->getDoubVal() + rul->getRuntime()));
                            row[4] = std::shared_ptr<CellInterface>(
                                    new Cell<long>(row[4]->getLongVal() + rul->getNum_tuples()));
                            row[0] = std::shared_ptr<CellInterface>(new Cell<double>(rul->getRuntime()));
                            rule_map[strTemp] = std::make_shared<Row>(row);
                        } else {
                            Row row(10);
                            row[1] = std::shared_ptr<CellInterface>(new Cell<double>(0.0));
                            row[2] = std::shared_ptr<CellInterface>(new Cell<double>(rul->getRuntime()));
                            row[4] = std::shared_ptr<CellInterface>(new Cell<long>(rul->getNum_tuples()));
                            row[5] = std::shared_ptr<CellInterface>(new Cell<std::string>(rul->getName()));
                            row[6] = std::shared_ptr<CellInterface>(new Cell<std::string>(rul->getId()));
                            row[7] = std::shared_ptr<CellInterface>(new Cell<std::string>(rel->getName()));
                            row[8] = std::shared_ptr<CellInterface>(new Cell<long>(rul->getVersion()));
                            row[9] = std::shared_ptr<CellInterface>(new Cell<std::string>(rul->getLocator()));
                            row[0] = std::shared_ptr<CellInterface>(new Cell<double>(rul->getRuntime()));
                            rule_map[strTemp] = std::make_shared<Row>(row);
                        }
                    }
                }
            }
            for (auto row : rule_map) {
                Row t = *row.second;
                double d = tot_rec_tup;
                long d2 = t[4]->getLongVal();
                t[3] = std::shared_ptr<CellInterface>(new Cell<double>(d2 * tot_copy_time / d));
                t[0] = std::shared_ptr<CellInterface>(
                        new Cell<double>(t[1]->getDoubVal() + t[2]->getDoubVal() + t[3]->getDoubVal()));
                rule_map[row.first] = std::make_shared<Row>(t);
            }
            break;
        }
    }

    Table table;
    for (auto& _row : rule_map) {
        table.addRow(_row.second);
    }
    return table;
}
