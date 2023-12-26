//
// Created by changxing on 2023/9/15.
//

#include "mars_ext_custom_morph_jni.h"
#include "extensions/morph/custom_morph.hpp"
#include "platform/android/jni_bridge.h"

extern "C"
JNIEXPORT jlong JNICALL
Java_com_alipay_mobile_mars_player_extensions_MarsCustomMorphExtension_createCustomMorphPlugin(
        JNIEnv* env, jobject thiz, jobjectArray jparams) {
    jsize length = env->GetArrayLength(jparams);
    std::vector<std::string> params;
    for (int i = 0; i < length; i++) {
        jstring jstr = (jstring) env->GetObjectArrayElement(jparams, i);
        const char* str = env->GetStringUTFChars(jstr, NULL);
        params.push_back(str);
        env->ReleaseStringUTFChars(jstr, str);
    }

    jclass clazz = mn::JNIUtil::FindClass("com/alipay/mobile/mars/player/extensions/MarsCustomMorphExtension");
    jmethodID mid = env->GetMethodID(clazz, "generateMorphDataFromNative", "(Ljava/lang/String;F)[Ljava/lang/Object;");
    if (!mid) {
        env->ExceptionClear();
        MLOGE("get generateMorphDataFromNative mid fail");
        return 0;
    }
    jobject global_thiz = env->NewGlobalRef(thiz);
    MLOGE("createCustomMorphPlugin %p %p %p", env, global_thiz, mid);
    return (jlong) new mn::CustomMorphPluginBuilder(params, [global_thiz, mid](const std::string& name, float time, float** points, int* points_length, uint16_t** indices, int* indices_length) {
        JNIEnv* env = mn::JNIUtil::GetEnv();
        if (!points) {
            env->DeleteGlobalRef(global_thiz);
            return;
        }
        jstring jstr = env->NewStringUTF(name.c_str());
        jobjectArray jarr = (jobjectArray) env->CallObjectMethod(global_thiz, mid, jstr, time);
        env->DeleteLocalRef(jstr);

        if (!jarr) {
            MLOGE("generateMorphDataFromNative empty %p %p %p", env, global_thiz, mid);
            return;
        }

        jfloatArray jpoints = (jfloatArray) env->GetObjectArrayElement(jarr, 0);
        jshortArray jindices = (jshortArray) env->GetObjectArrayElement(jarr, 1);

        *points_length = (int) env->GetArrayLength(jpoints);
        *points = new float[*points_length];
        *indices_length = (int) env->GetArrayLength(jindices);
        *indices = new uint16_t[*indices_length];
        {
            jfloat* floatArray = env->GetFloatArrayElements(jpoints, NULL);
            memcpy(*points, floatArray, (*points_length) * sizeof(float));
            env->ReleaseFloatArrayElements(jpoints, floatArray, 0);
        }
        {
            jshort* intArray = env->GetShortArrayElements(jindices, NULL);
            memcpy(*indices, intArray, (*indices_length) * sizeof(jshort));
            env->ReleaseShortArrayElements(jindices, intArray, 0);
        }

        env->DeleteLocalRef(jarr);
        env->DeleteLocalRef(jpoints);
        env->DeleteLocalRef(jindices);
    });
}