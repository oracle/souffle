#include "com_soufflelang_souffle_Result.h"
#include "handle.h"
#include "InterfaceResult.h"
#include <iostream>
#include <jni.h>
#include "conversion.h"

using namespace souffle;

void Java_com_soufflelang_souffle_Result_release(JNIEnv* env, jobject obj) {
    LOG(INFO) ENTERJNI("release");

    InterfaceResult* data = getHandle<InterfaceResult>(env, obj);

    LOG(MEM) PRE << "Got and releasing Interface Result " << data << "\n";
    assert(data != NULL && "Interface result is NULL!!!\n");

    LOG(INFO) LEAVEJNI;
    delete data;
}

jobject Java_com_soufflelang_souffle_Result_getRelationNames(JNIEnv* env, jobject obj) {
    LOG(INFO) ENTERJNI("getRelationNames");

    InterfaceResult* intres = getHandle<InterfaceResult>(env, obj);
    LOG(MEM) PRE << "Got Interface Result " << intres << "\n";
    assert(intres != NULL && "Interface result is NULL!!!\n");

    std::vector<std::string> vec = intres->getRelationNames();
    LOG(INFO) PRE << "Got Relation names. Size of names is : " << vec.size() << "\n";

    jclass java_util_ArrayList2 = 
      static_cast<jclass>(env->NewGlobalRef(env->FindClass("java/util/ArrayList")));
 
    if (java_util_ArrayList2 == 0){
      LOG(MEM) PRE << "Cannot find class java.util.ArrayList\n";
      assert(false);
    }

    jmethodID java_util_ArrayList_2 = env->GetMethodID(java_util_ArrayList2,"<init>", "(I)V");
    if (java_util_ArrayList_2 == 0){
      LOG(ERR) PRE << "cannot find method java.util.arraylist <init>\n";
      assert(false);
    }

    jmethodID java_util_ArrayList_add2 = 
      env->GetMethodID(java_util_ArrayList2, "add", "(Ljava/lang/Object;)Z");
    if (java_util_ArrayList_add2 == 0){
      LOG(ERR) PRE << "Cannot find method java.util.ArrayList add\n";
      assert(false);
    }

    jobject result = 
      env->NewObject(java_util_ArrayList2, java_util_ArrayList_2, (jint)vec.size());

    for (auto &v: vec) {
      env->CallVoidMethod(result, java_util_ArrayList_add2, env->NewStringUTF(v.c_str()));
    }
    LOG(INFO) LEAVEJNI;
    return result;
}

jobject Java_com_soufflelang_souffle_Result_getPrimData(JNIEnv* env, jobject obj, jstring str) {
    LOG(INFO) ENTERJNI("getPrimData");
    std::string name = std::string(env->GetStringUTFChars(str, 0));
    LOG(INFO) PRE << "Relation name is " << name << "\n";

    InterfaceResult* intres = getHandle<InterfaceResult>(env, obj);
    if(intres == NULL){
      LOG(ERR) PRE << "Interface result is null\n";
      assert(intres != NULL && "Interface result is null!!");
    }

    PrimData* pdata = intres->getPrimRelation(name);
    if(pdata == NULL){
      LOG(WARN) PRE << "Prim data is null\n";
    }

    // Create PrimData java Object and add pdata to its oconstructor
    jclass c = env->FindClass("com/soufflelang/souffle/PrimData");
    if (c == 0) printf("Find class failed.\n");
    jmethodID cnstrctr = env->GetMethodID(c, "<init>", "(J)V");
    if (cnstrctr == 0) printf("Find method failed.\n");
    LOG(INFO) LEAVEJNI;
    return env->NewObject(c, cnstrctr, pdata);
}

jobject Java_com_soufflelang_souffle_Result_getRelationRows(JNIEnv* env, jobject obj, jstring str) {
    LOG(INFO) ENTERJNI("getRelationRows");
    std::string name = std::string(env->GetStringUTFChars(str, 0));
    InterfaceResult* intres = getHandle<InterfaceResult>(env, obj);
    if(intres == NULL){
      LOG(ERR) PRE << "Interface result is null\n";
      assert(intres != NULL && "Interface result is null!!");
    }

    PrimData* pdata = intres->getPrimRelation(name);
    if(pdata == NULL){
      LOG(WARN) PRE << "Prim data is null\n";
    }

    jclass java_util_ArrayList2 = 
      static_cast<jclass>(env->NewGlobalRef(env->FindClass("java/util/ArrayList")));

    jmethodID java_util_ArrayList_2 = env->GetMethodID(java_util_ArrayList2,"<init>", "(I)V");

    jmethodID java_util_ArrayList_add2 = 
      env->GetMethodID(java_util_ArrayList2, "add", "(Ljava/lang/Object;)Z");

    if (pdata == NULL) {
       LOG(WARN) PRE << "cannot get relation " << name << "\n";
       jobject result = env->NewObject(java_util_ArrayList2, java_util_ArrayList_2, (jint)0);
       return result;
    }

    jobject result2 = 
      env->NewObject(java_util_ArrayList2, java_util_ArrayList_2, (jint)pdata->data.size());

    for (std::vector<std::string>v: pdata->data) {
      env->CallVoidMethod(result2, java_util_ArrayList_add2, vec2arr(env, v));
    }

    LOG(INFO) LEAVEJNI;
    return result2;
}
