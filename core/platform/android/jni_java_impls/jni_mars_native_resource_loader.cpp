#include <jni.h>
#include "player/mars_player_mgr.h"
#include "platform/android/jni_bridge.h"

//
// Created by changxing on 2023/9/30.
//

extern "C"
JNIEXPORT jlong JNICALL
Java_com_alipay_mobile_mars_util_JNIUtil_nativeCreateVideoContextAndPrepare(
        JNIEnv *env, jclass clazz_nouse, jint player_idx, jstring j_file_path, jstring j_key, jboolean transparent, jboolean hard_decoder, jobject callback) {
    const char* file_path = env->GetStringUTFChars(j_file_path, nullptr);
    const char* key = env->GetStringUTFChars(j_key, nullptr);

    jclass clazz = mn::JNIUtil::FindClass("com/alipay/mobile/mars/util/JNIUtil$CommonJNICallback");
    if (!clazz) {
        MLOGE("nativeCreateVideoContextAndPrepare no clazz");
        return 0;
    }
    jmethodID mid = env->GetMethodID(clazz, "onCallback", "([Ljava/lang/String;)V");
    if (!mid) {
        MLOGE("nativeCreateVideoContextAndPrepare no mid");
        return 0;
    }

    jobject global_callback = env->NewGlobalRef(callback);
    auto vc = mn::MarsPlayerManager::CreateVideoContextAndPrepare(
            player_idx, file_path, key, transparent == JNI_TRUE, hard_decoder == JNI_TRUE,
            [global_callback, mid](bool success, const std::string& msg) {
                MLOGD("Prepare Video: %d %s", success, msg.c_str());
                JNIEnv *env = mn::JNIUtil::GetEnv();
                jclass stringClass = env->FindClass("java/lang/String");
                jobjectArray params = env->NewObjectArray(2, stringClass, nullptr);
                jstring result = success ? env->NewStringUTF("true") : env->NewStringUTF("false");
                jstring err = env->NewStringUTF(msg.c_str());
                env->SetObjectArrayElement(params, 0, result);
                env->SetObjectArrayElement(params, 1, err);
                env->CallVoidMethod(global_callback, mid, params);

                env->DeleteLocalRef(params);
                env->DeleteLocalRef(result);
                env->DeleteLocalRef(err);
                env->DeleteGlobalRef(global_callback);
            }
    );

    env->ReleaseStringUTFChars(j_file_path, file_path);
    env->ReleaseStringUTFChars(j_key, key);

    return (jlong) vc;
}
