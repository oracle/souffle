


#include <iostream>
#include <string>

#include "profilerlib/Cli.hpp"


 int main(int argc, char* argv[]) {
    Cli cli_obj = Cli(argc, argv);
    cli_obj.parse();
    std::cout << "\r";
    return 0;
 }

