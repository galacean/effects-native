//
// Created by changxing on 2023/9/13.
//

#ifndef ANDROID_MARS_EXT_POSITION_PATH_MODIFIER_JNI_H
#define ANDROID_MARS_EXT_POSITION_PATH_MODIFIER_JNI_H

#include <jni.h>

extern "C"
JNIEXPORT jlong JNICALL
Java_com_alipay_mobile_mars_player_extensions_MarsPositionPathModifierExtension_CreateExtPositionPathModifier(
        JNIEnv* env, jobject thiz);

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_alipay_mobile_mars_player_extensions_MarsPositionPathModifierExtension_ExtPositionPathModifierSetSceneData(
        JNIEnv* env, jobject thiz, jlong modifier, jlong scene_data);

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_alipay_mobile_mars_player_extensions_MarsPositionPathModifierExtension_ExtPositionPathModifierGetPositionPathByName(
        JNIEnv* env, jobject thiz, jlong modifier, jstring name);

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_com_alipay_mobile_mars_player_extensions_MarsPositionPathModifierExtension_ExtPositionPathModifierGetParams(
        JNIEnv* env, jobject thiz, jlong modifier);

extern "C"
JNIEXPORT void JNICALL
Java_com_alipay_mobile_mars_player_extensions_MarsPositionPathModifierExtension_ExtPositionPathModifierModifyPositionPath(
        JNIEnv *env, jobject thiz, jlong modifier, jstring name, jfloatArray data,
        jfloatArray data2);

extern "C"
JNIEXPORT void JNICALL
Java_com_alipay_mobile_mars_player_extensions_MarsPositionPathModifierExtension_ExtPositionPathModifierDestroy(
        JNIEnv *env, jobject thiz, jlong modifier);

#endif //ANDROID_MARS_EXT_POSITION_PATH_MODIFIER_JNI_H
