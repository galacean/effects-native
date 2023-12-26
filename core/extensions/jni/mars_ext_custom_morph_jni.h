//
// Created by changxing on 2023/9/15.
//

#ifndef ANDROID_MARS_EXT_CUSTOM_MORPH_JNI_H
#define ANDROID_MARS_EXT_CUSTOM_MORPH_JNI_H

#include <jni.h>

extern "C"
JNIEXPORT jlong JNICALL
Java_com_alipay_mobile_mars_player_extensions_MarsCustomMorphExtension_createCustomMorphPlugin(
        JNIEnv* env, jobject thiz, jobjectArray params);

#endif //ANDROID_MARS_EXT_CUSTOM_MORPH_JNI_H
