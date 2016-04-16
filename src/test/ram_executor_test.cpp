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
 * @file type_system_test.h
 *
 * Tests souffle's type system operations.
 *
 ***********************************************************************/

#include "test.h"

#include "AstProgram.h"
#include "RamTranslator.h"
#include "RamExecutor.h"
#include "RamStatement.h"

namespace souffle {
namespace test {

    TEST(Ast, CloneAndEquals) {

        // TODO: add test for records

        // load some test program
        AstProgram program = AstProgram::parse(
            R"(
                 .number_type N
                 .decl e( a : N, b : N )
                 .decl l( a : N, b : N ) output

                 e(1,2).
                 e(2,3).
                 e(3,4).

                 l(a,b) :- e(a,b).
                 l(a,c) :- e(a,b), l(b,c).
            )"
        );

        // parse program
        EXPECT_EQ(program, program);
//        std::cout << program << "\n";

        // translate AST to RAM
        auto ram_prog = RamTranslator().translateProgram(program);
//        std::cout << *ram_prog << "\n";

        // execute RAM program
        RamEnvironment env;
        RamInterpreter executor;
        executor.getConfig().setOutputDir("-");
        executor.applyOn(*ram_prog, env);

        ASSERT_TRUE(env.hasRelation("l"));
        auto& rel = env.getRelation("l");

//        for(const auto& cur : rel) {
//            for(unsigned i=0; i<rel.getArity(); i++) {
//                std::cout << cur[i] << " ";
//            }
//            std::cout << "\n";
//        }

        EXPECT_EQ(6, rel.size());

        // done
        delete ram_prog;
    }

} // end namespace test
} // end namespace souffle

