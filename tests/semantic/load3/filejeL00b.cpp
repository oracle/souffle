#include "souffle/CompiledSouffle.h"

namespace souffle {
using namespace ram;
class Sf_filejeL00b : public SouffleProgram {
private:
static bool regex_wrapper(const char *pattern, const char *text) {
   bool result = false; 
   try { result = std::regex_match(text, std::regex(pattern)); } catch(...) { 
     std::cerr << "warning: wrong pattern provided for match(\"" << pattern << "\",\"" << text << "\")\n";
}
   return result;
}
public:
SymbolTable symTable;
// -- Table: A
ram::Relation<2> rel_A;
souffle::RelationWrapper<0,ram::Relation<2>,Tuple<RamDomain,2>,2,true,true> wrapper_A;
public:
Sf_filejeL00b() : 
wrapper_A(rel_A,symTable,"A",std::array<const char *,2>{"i:number","i:number"},std::array<const char *,2>{"x","y"}){
addRelation("A",&wrapper_A,1,0);
}
void run() {
// -- initialize counter --
std::atomic<RamDomain> ctr(0);

#if defined(__EMBEDDED_SOUFFLE__) && defined(_OPENMP)
omp_set_num_threads(1);
#endif

// -- query evaluation --
}
public:
void printAll(std::string dirname=".") {
{ auto lease = getOutputLock().acquire(); 
std::cout << R"(A	)" <<  rel_A.size() << "\n";
}}
public:
void loadAll(std::string dirname=".") {
rel_A.loadCSV(dirname + "/A.facts",symTable,0,0);
}
public:
void dumpInputs(std::ostream& out = std::cout) {
out << "---------------\nA\n===============\n";
rel_A.printCSV(out,symTable,0,0);
out << "===============\n";
}
public:
void dumpOutputs(std::ostream& out = std::cout) {
}
public:
const SymbolTable &getSymbolTable() const {
return symTable;
}
};
SouffleProgram *newInstance_filejeL00b(){return new Sf_filejeL00b;}
SymbolTable *getST_filejeL00b(SouffleProgram *p){return &reinterpret_cast<Sf_filejeL00b*>(p)->symTable;}
#ifdef __EMBEDDED_SOUFFLE__
class factory_Sf_filejeL00b: public souffle::ProgramFactory {
SouffleProgram *newInstance() {
return new Sf_filejeL00b();
};
public:
factory_Sf_filejeL00b() : ProgramFactory("filejeL00b"){}
};
static factory_Sf_filejeL00b __factory_Sf_filejeL00b_instance;
}
#else
}
int main(int argc, char** argv)
{
souffle::CmdOptions opt(R"(load3.dl)",
R"(facts)",
R"(.)",
false,
R"()",
1,
R"(false)");
if (!opt.parse(argc,argv)) return 1;
#if defined(_OPENMP) 
omp_set_nested(true);
#endif
souffle::Sf_filejeL00b obj;
obj.loadAll(opt.getInputFileDir());
obj.run();
if (!opt.getOutputFileDir().empty()) obj.printAll(opt.getOutputFileDir());
return 0;
}
#endif
