//
// Created by changxing on 2023/9/13.
//

#include "mars_ext_position_path_modifier_jni.h"
#include "extensions/mars_ext_position_path_modifier.hpp"

extern "C"
JNIEXPORT jlong JNICALL
Java_com_alipay_mobile_mars_player_extensions_MarsPositionPathModifierExtension_CreateExtPositionPathModifier(
        JNIEnv* env, jobject thiz) {
    return (jlong) (new mn::MarsExtPositionPathModifier());
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_alipay_mobile_mars_player_extensions_MarsPositionPathModifierExtension_ExtPositionPathModifierSetSceneData(
        JNIEnv* env, jobject thiz, jlong jmodifier, jlong jscene_data) {
    mn::MarsExtPositionPathModifier* modifier = (mn::MarsExtPositionPathModifier*) jmodifier;
    mn::SceneDataDecoder* scene_data = (mn::SceneDataDecoder*) jscene_data;
    bool result = modifier->SetSceneData(scene_data);
    return result ? JNI_TRUE : JNI_FALSE;
}

extern "C"
JNIEXPORT jobjectArray JNICALL
Java_com_alipay_mobile_mars_player_extensions_MarsPositionPathModifierExtension_ExtPositionPathModifierGetPositionPathByName(
        JNIEnv* env, jobject thiz, jlong jmodifier, jstring jname) {
    mn::MarsExtPositionPathModifier* modifier = (mn::MarsExtPositionPathModifier*) jmodifier;
    const char* name = env->GetStringUTFChars(jname, NULL);

    float* data = nullptr;
    float* data2 = nullptr;
    int length = 0;
    int length2 = 0;
    bool result = modifier->GetPositionPathByName(name, &data, &length, &data2, &length2);
    env->ReleaseStringUTFChars(jname, name);

    if (!result) {
        return nullptr;
    }
    jfloatArray array = env->NewFloatArray(length);
    env->SetFloatArrayRegion(array, 0, length, data);
    jfloatArray array2 = nullptr;
    if (data2) {
        array2 = env->NewFloatArray(length2);
        env->SetFloatArrayRegion(array2, 0, length2, data2);
    }

    jclass floatArrayClass = env->FindClass("[F");
    jobjectArray resultArray = env->NewObjectArray(2, floatArrayClass, NULL);
    env->SetObjectArrayElement(resultArray, 0, array);
    env->SetObjectArrayElement(resultArray, 1, array2);

    env->DeleteLocalRef(array);
    env->DeleteLocalRef(array2);

    return resultArray;
}

extern "C"
JNIEXPORT jfloatArray JNICALL
Java_com_alipay_mobile_mars_player_extensions_MarsPositionPathModifierExtension_ExtPositionPathModifierGetParams(
        JNIEnv* env, jobject thiz, jlong jmodifier) {
    mn::MarsExtPositionPathModifier* modifier = (mn::MarsExtPositionPathModifier*) jmodifier;
    jfloatArray array = env->NewFloatArray(4);
    float pixel_ratio = modifier->GetPixelRatio();
    float aspect = modifier->GetAspect();
    float anim_width = modifier->GetAnimWidth();
    float anim_height = modifier->GetAnimHeight();
    env->SetFloatArrayRegion(array, 0, 1, &pixel_ratio);
    env->SetFloatArrayRegion(array, 1, 1, &aspect);
    env->SetFloatArrayRegion(array, 2, 1, &anim_width);
    env->SetFloatArrayRegion(array, 3, 1, &anim_height);
    return array;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_alipay_mobile_mars_player_extensions_MarsPositionPathModifierExtension_ExtPositionPathModifierModifyPositionPath(
        JNIEnv *env, jobject thiz, jlong jmodifier, jstring jname, jfloatArray jdata,
        jfloatArray jdata2) {
    mn::MarsExtPositionPathModifier* modifier = (mn::MarsExtPositionPathModifier*) jmodifier;
    const char* name = env->GetStringUTFChars(jname, NULL);

    jfloat* floatArray = env->GetFloatArrayElements(jdata, NULL);
    int length = (int) env->GetArrayLength(jdata);
    float* data = (float*) malloc(length * sizeof(float));
    memcpy(data, floatArray, length * sizeof(float));
    env->ReleaseFloatArrayElements(jdata, floatArray, 0);

    float* data2 = nullptr;
    int length2 = 0;
    if (jdata2) {
        jfloat *floatArray2 = env->GetFloatArrayElements(jdata2, NULL);
        length2 = (int) env->GetArrayLength(jdata2);
        data2 = (float *) malloc(length2 * sizeof(float));
        memcpy(data2, floatArray2, length2 * sizeof(float));
        env->ReleaseFloatArrayElements(jdata2, floatArray2, 0);
    }

    modifier->ModifyPositionPath(name, data, length, data2, length2);

    env->ReleaseStringUTFChars(jname, name);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_alipay_mobile_mars_player_extensions_MarsPositionPathModifierExtension_ExtPositionPathModifierDestroy(
        JNIEnv *env, jobject thiz, jlong jmodifier) {
    mn::MarsExtPositionPathModifier* modifier = (mn::MarsExtPositionPathModifier*) jmodifier;
    delete modifier;
}