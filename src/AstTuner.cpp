/*
 * Copyright (c) 2013, 2015, Oracle and/or its affiliates. All Rights reserved
 * 
 * The Universal Permissive License (UPL), Version 1.0
 * 
 * Subject to the condition set forth below, permission is hereby granted to any person obtaining a copy of this software,
 * associated documentation and/or data (collectively the "Software"), free of charge and under any and all copyright rights in the 
 * Software, and any and all patent rights owned or freely licensable by each licensor hereunder covering either (i) the unmodified 
 * Software as contributed to or provided by such licensor, or (ii) the Larger Works (as defined below), to deal in both
 * 
 * (a) the Software, and
 * (b) any piece of software and/or hardware listed in the lrgrwrks.txt file if one is included with the Software (each a “Larger
 * Work” to which the Software is contributed by such licensors),
 * 
 * without restriction, including without limitation the rights to copy, create derivative works of, display, perform, and 
 * distribute the Software and make, use, sell, offer for sale, import, export, have made, and have sold the Software and the 
 * Larger Work(s), and to sublicense the foregoing rights on either these or other terms.
 * 
 * This license is subject to the following condition:
 * The above copyright notice and either this complete permission notice or at a minimum a reference to the UPL must be included in 
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
 * IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/************************************************************************
 *
 * @file AstTuner.h
 *
 * Provides utilities to conduct automated tuning of AstStructures.
 *
 ***********************************************************************/

#include "AstTuner.h"
#include "AstProgram.h"
#include "AstVisitor.h"

#include "RamStatement.h"
#include "RamTranslator.h"
#include "RamExecutor.h"

namespace souffle {

namespace {

    /**
     * A execution strategy wrapper for the guided interpreter recording
     * execution times and scheduling decisions.
     */
    class Profiler {

    public:

        /** The type of data to be recorded -- Clauses are identified by ther source location */
        typedef std::map<AstSrcLocation, std::vector<ExecutionSummary>> Data;

    private:

        /** The nested scheduler */
        const QueryExecutionStrategy& nested;

        /** The recorded data */
        Data& data;

    public:

        Profiler(const QueryExecutionStrategy& strategy, Data& data)
            : nested(strategy), data(data) {}

        /**
         * Processes the given query by forwarding the call to the nested strategy an
         * recording its performance.
         */
        ExecutionSummary operator()(const RamExecutorConfig& config, const RamInsert& insert, RamEnvironment& env, std::ostream* report) const {

            // run nested strategy
            auto res = nested(config, insert, env, report);

            // record the execution summary
            data[insert.getOrigin().getSrcLoc()].push_back(res);

            // act like a wrapper
            return res;
        }
    };

}

bool AutoScheduleTransformer::transform(AstTranslationUnit& translationUnit) {
    bool changed = false;
    if (generateScheduleReport) {
        std::stringstream report;
        changed = autotune(translationUnit, factDir, &report, verbose);
        translationUnit.getDebugReport().addSection(DebugReporter::getCodeSection("auto-schedule", "Auto Schedule Report", report.str()));
    } else {
        changed = autotune(translationUnit, factDir, nullptr, verbose);
    }
    return changed;
}

bool AutoScheduleTransformer::autotune(AstTranslationUnit& translationUnit, const std::string& factDir, std::ostream* report, bool verbose, const QueryExecutionStrategy& strategy) {

    // start with status message
    if (verbose) std::cout << "\n";
    if (verbose) std::cout << "----------------- Auto-Scheduling Started -----------------\n";

    // step 1 - translate to RAM program
    if (verbose) std::cout << "[ Converting to RAM Program ...                           ]\n";
    std::unique_ptr<RamStatement> stmt = RamTranslator().translateProgram(translationUnit);

    // check whether there is something to tune
    if (!stmt) {
        if (verbose) std::cout << "[                                     No Rules in Program ]\n";
        if (verbose) std::cout << "---------------- Auto-Scheduling Completed ----------------\n";
        return false;
    }

    if (verbose) std::cout << "[                                                    Done ]\n";


    // step 2 - run in interpreted mode, collect decisions
    if (verbose) std::cout << "[ Profiling RAM Program ...                               ]\n";

    Profiler::Data data;
    Profiler profiler(strategy, data);

    // create a copy of the symbol table
    souffle::SymbolTable table = translationUnit.getSymbolTable();

    // create interpreter instance
    RamGuidedInterpreter interpreter(profiler);
    interpreter.getConfig().setFactFileDir(factDir);

    if (report && verbose) {
        SplitStream splitStream(report, &std::cout);
        interpreter.setReportTarget(splitStream);
    } else if (report) {
        interpreter.setReportTarget(*report);
    } else if (verbose) {
        interpreter.setReportTarget(std::cout);
    }

    // run interpreter
    interpreter.execute(table, *stmt);

    if (verbose) std::cout << "[                                                    Done ]\n";

    if (verbose) { 
        std::cout << "Data:\n";
        for(const auto& cur : data) {
            std::cout << "Clause @ " << cur.first << "\n";
            for(const ExecutionSummary& instance : cur.second) {
                std::cout << "\t" << instance.order << " in " << instance.time << "ms\n";
            }
        }
    } 

    // step 3 - process collected data ..
    if (verbose) std::cout << "[ Selecting most significant schedules ...                ]\n";

    std::map<AstSrcLocation, const AstClause*> clauses;
    visitDepthFirst(*translationUnit.getProgram(), [&](const AstClause& clause) {
        clauses[clause.getSrcLoc()] = &clause;
    });

    std::map<const AstClause*, long> longestTime;
    std::map<const AstClause*, Order> bestOrders;

    // extract best order for each clause
    for(const auto& cur : data) {
        const AstClause* clause = clauses[cur.first];
        assert(clause && "Unknown clause discovered!");
        for(const ExecutionSummary& instance : cur.second) {
            if (longestTime[clause] < instance.time) {
                longestTime[clause] = instance.time;
                bestOrders[clause] = instance.order;
            }
        }
    }


    if (verbose) { 
        for(const auto& cur : bestOrders) {
            std::cout << *cur.first << "\n Best Order: " << cur.second << "\n Time: " << longestTime[cur.first] << "\n\n";
        }
    }

    if (verbose) std::cout << "[                                                    Done ]\n";

    // step 4 - apply transformations
    if (verbose) std::cout << "[ Re-scheduling rules ...                                 ]\n";

    bool changed = false;
    for(const auto& cur : bestOrders) {
        AstClause* clause = const_cast<AstClause*>(cur.first);
        bool orderChanged = false;
        const std::vector<unsigned> &newOrder = cur.second.getOrder();

        // Check whether best order is different to the original order
        for (unsigned int i = 0; i < clause->getAtoms().size(); i++) {
            if (newOrder[i] != i) {
                orderChanged = true;
                break;
            }
        }
        if (orderChanged) {
            clause->reorderAtoms(newOrder);
            changed = true;
        }
    }

    if (verbose) std::cout << "[                                                    Done ]\n";

    // end with status message
    if (verbose) std::cout << "---------------- Auto-Scheduling Completed ----------------\n";

    return changed;
}

} // end of namespace souffle

