


#include <iostream>
#include <string>

// #include "profilerlib/Cli.hpp"


// int main(int argc, char* argv[]) {
//    Cli cli_obj = Cli(argc, argv);
//    cli_obj.parse();
//    std::cout << "clean exit.\n";
//    return 0;
// }


// #include "profilerlib/Rule.hpp"

// int main() {

// 	Rule r1("R1","1"),
// 		r2("R2",1,"2"),
// 		r3("R3","3");

// 	std::cout << r1.toString();
// }

// #include "profilerlib/Relation.hpp"

// int main() {
//     Iteration x = Iteration();

// }


// #include "profilerlib/Iteration.hpp"

// int main() {
//     Iteration x = Iteration();
//     x.insert("hello", "world");
//     std::cout << x.get("hello");
// }


#include "profilerlib/Reader.hpp"
#include "profilerlib/ProgramRun.hpp"

int main() {
	ProgramRun x = ProgramRun();
    Reader read = Reader("/Users/Dom/souffle_test/prof1.prof", x, false, false);
    read.readFile();
}