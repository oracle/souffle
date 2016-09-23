#include "com_soufflelang_souffle_Data.h"
#include "handle.h"
#include "conversion.h"
#include "RamData.h"
#include <iostream>

using namespace souffle;

void Java_com_soufflelang_souffle_Data_init(JNIEnv* env, jobject obj) {
    LOG(INFO) ENTERJNI("init");

    RamData* data = new RamData();
    LOG(MEM) PRE << "Created data object "<< data << "\n";
    setHandle(env, obj, data);

    LOG(INFO) LEAVEJNI;
}

void Java_com_soufflelang_souffle_Data_release(JNIEnv* env, jobject obj) {
    LOG(INFO) ENTERJNI("release");

    RamData* data = getHandle<RamData>(env, obj);
    LOG(MEM) PRE << "Retrieved data object "  << data << "\n";

    LOG(INFO) LEAVEJNI;
    delete data;
}

void Java_com_soufflelang_souffle_Data_print(JNIEnv* env, jobject obj) {
    LOG(INFO) ENTERJNI("print");
    RamData* data = getHandle<RamData>(env, obj);
    LOG(MEM) PRE << "Retrieved data object "  << data << "\n";
    //data->print();
    LOG(INFO) LEAVEJNI;
}

jobject Java_com_soufflelang_souffle_Data_merge(JNIEnv* env, jobject d1, jobject d2) {
    LOG(INFO) ENTERJNI("merge");
    RamData* data1 = getHandle<RamData>(env, d1);
    RamData* data2 = getHandle<RamData>(env, d2);

    RamData* data3 = data1->merge(data2);

    jclass c = env->FindClass("com/soufflelang/souffle/Data");
    if (c == 0) {
      LOG(ERR) PRE << "Cannot Find class Data" << "\n";
      assert(false);
    }

    jmethodID cnstrctr = env->GetMethodID(c, "<init>", "(J)V");
    if (cnstrctr == 0) {
      LOG(ERR) PRE << "Cannot find method Data <init>" << "\n";
      assert(false);
    }

    LOG(INFO) LEAVEJNI;
    return env->NewObject(c, cnstrctr, data3);
}

void Java_com_soufflelang_souffle_Data_addRelationTuple(JNIEnv* env, jobject obj, jstring str, jobject obj2) {
    LOG(INFO) ENTERJNI("addRelationTuple");
    std::string name = std::string(env->GetStringUTFChars(str, 0));
    LOG(MEM) PRE << "RamData for relation " << name << "\n";

    RamData* data = getHandle<RamData>(env, obj);
    LOG(MEM) PRE << "Retrieved RamData " << data << "\n";
    assert(data != NULL && "Data is null!!\n");

    std::vector<std::string> arr = arr2vec(env, obj2);
    data->addTuple(name, arr);
    LOG(INFO) PRE << "Added tuple to data to relation " << name << " of size " << arr.size() << "\n";
    LOG(INFO) LEAVEJNI;
}

void Java_com_soufflelang_souffle_Data_addRelationData(JNIEnv* env, jobject obj, jstring str, jobject obj2) {
    LOG(INFO) ENTERJNI("addRelationData");
    std::string name = std::string(env->GetStringUTFChars(str, 0));
    LOG(MEM) PRE << "RamData for relation " << name << "\n";

    RamData* data = getHandle<RamData>(env, obj);
    assert(data != NULL);

    LOG(MEM) PRE << "Got RamData " << data << "\n";
    PrimData* pdata = getHandle<PrimData>(env, obj2);

    LOG(MEM) PRE << "Got PrimData " << pdata << "\n";
    assert(pdata != NULL && "pdata is null!\n");

    data->addTuples(name, pdata);
    LOG(INFO) PRE << "Added tuple to data to relation " << name << " with data " << pdata << "\n";
    LOG(INFO) LEAVEJNI;
}

