#include "Interfaces.h"

using namespace souffle;

Executor* InternalInterface::parse(AstBuilder* driver) {
    LOG(INFO) ENTERCPP("parse");
    AstTranslationUnit* translationUnit = driver->getTranslationUnit();
    if(translationUnit == NULL) { 
      std::cout << "no translation unit \n";
      LOG(ERR) PRE << "No translation unit\n";
      assert(false && "No translation unit\n");
      return NULL;
    }

    // ------- rewriting / optimizations -------------
    std::vector<std::unique_ptr<AstTransformer>> transforms;
    transforms.push_back(std::unique_ptr<AstTransformer>(new ComponentInstantiationTransformer()));
        LOG(INFO) PRE << "Added Component Instantiation Transformer\n";
    transforms.push_back(std::unique_ptr<AstTransformer>(new UniqueAggregationVariablesTransformer()));
        LOG(INFO) PRE << "Added Unique Aggregation Variables Transformer\n";
    transforms.push_back(std::unique_ptr<AstTransformer>(new AstSemanticChecker()));
        LOG(INFO) PRE << "Added Ast Semantic Checker Transformer\n";
    transforms.push_back(std::unique_ptr<AstTransformer>(new ResolveAliasesTransformer()));
        LOG(INFO) PRE << "Added Resolve Aliases Transformer\n";
    transforms.push_back(std::unique_ptr<AstTransformer>(new RemoveRelationCopiesTransformer()));
        LOG(INFO) PRE << "Added Remove Copies Transformer\n";
    transforms.push_back(std::unique_ptr<AstTransformer>(new MaterializeAggregationQueriesTransformer()));
        LOG(INFO) PRE << "Added Aggregation Queries Transformer\n";
    transforms.push_back(std::unique_ptr<AstTransformer>(new RemoveEmptyRelationsTransformer()));
        LOG(INFO) PRE << "Added Remove Empty Relations Transformer\n";
   
    for (const auto &transform : transforms) {
        LOG(INFO) PRE << "Applying transformation\n";
        transform->apply(*translationUnit);
    }

    /* translate AST to RAM */
    LOG(INFO) PRE << "Translating ram\n";
    std::unique_ptr<RamStatement> ramProgram = RamTranslator(true).translateProgram(*translationUnit);

    if(ramProgram == nullptr) {
        LOG(WARN) PRE << "Ram is empty!\n";
        return NULL;
    }

    LOG(INFO) LEAVECPP;
    return new Executor(translationUnit->getSymbolTable(), std::move(ramProgram));
}

