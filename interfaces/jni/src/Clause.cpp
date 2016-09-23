#include<jni.h>
#include "com_soufflelang_souffle_Clause.h"

#include "handle.h"
#include "AstClause.h"

using namespace souffle;

void Java_com_soufflelang_souffle_Clause_init(JNIEnv *env, jobject obj) {
    LOG(INFO) ENTERJNI("init");
    AstClause* clause = new AstClause();
    LOG(MEM) PRE << "Creating clause " << clause << "\n";
    setHandle(env, obj, clause);
    LOG(MEM) PRE << "Setting clause as handle " << clause << "\n";
    LOG(INFO) LEAVEJNI;
}

void JNICALL Java_com_soufflelang_souffle_Clause_setHead(JNIEnv* env, jobject obj1, jobject obj2) {
    LOG(INFO) ENTERJNI("setHead");
    AstClause *cl = getHandle<AstClause>(env, obj1);
    LOG(MEM) PRE << "Got clause " << cl << "\n";
    AstAtom *a = getHandle<AstAtom>(env, obj2);
    LOG(MEM) PRE << "Got atom " << a << "\n";
    cl->setHead(std::unique_ptr<AstAtom>(a));
    LOG(MEM) PRE << "Setting atom to clause as head atom \n";
    LOG(INFO) LEAVEJNI;
}

void JNICALL Java_com_soufflelang_souffle_Clause_addToBody(JNIEnv* env, jobject obj1, jobject obj2) {
    LOG(INFO) ENTERJNI("addToBody");
    AstClause* cl = getHandle<AstClause>(env, obj1);
    LOG(MEM) PRE << "Got clause " << cl << "\n";
    AstLiteral* l = getHandle<AstLiteral>(env, obj2);
    LOG(MEM) PRE << "Got literal " << l << "\n";
    cl->addToBody(std::unique_ptr<AstLiteral>(l));
    LOG(INFO) PRE << "set literal to clause body\n";
    LOG(INFO) LEAVEJNI;
}
