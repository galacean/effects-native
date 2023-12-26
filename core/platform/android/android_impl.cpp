#include <android/bitmap.h>
#include "util/image_loader.h"
#include "util/log_util.hpp"
#include "player/mars_player.h"
#include "platform/android/jni_bridge.h"
#include "sceneData/scene_data_decoder.h"

namespace mn {

ImageData* ImageLoader::ParseImageData(ImageRawData* raw_data) {
    if (raw_data->decoded) {
        ImageData* data = new ImageData();
        data->width = raw_data->width;
        data->height = raw_data->height;
        data->premultiply_alpha = raw_data->premultiply_alpha;
        data->data = (uint8_t*) malloc(data->width * data->height * 4);
        memcpy(data->data, raw_data->data, data->width * data->height * 4);
        return data;
    }
    JNIEnv* env = JNIUtil::GetEnv();
    jclass clazz = JNIUtil::GetEventEmiterClass();
    if (!clazz) {
        MLOGE("ParseImageData jclass not found");
        return nullptr;
    }
    jmethodID mid = JNIUtil::GetEventEmiterParseBitmapMID();
    if (!mid) {
        MLOGE("ParseImageData jmethodID not found");
        return nullptr;
    }
    jbyteArray jarr = env->NewByteArray(raw_data->byte_length);
    env->SetByteArrayRegion(jarr, 0, raw_data->byte_length, (jbyte*) raw_data->data);
    jobject bitmap = env->CallStaticObjectMethod(clazz, mid, jarr);
    AndroidBitmapInfo info;
    int result;

    result = AndroidBitmap_getInfo(env, bitmap, &info);
    if (result != ANDROID_BITMAP_RESUT_SUCCESS || info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        MLOGE("AndroidBitmap_getInfo failed, result: %d, format: %d", result, info.format);
        return nullptr;
    }

    MLOGD("ParseImageData width: %d, height: %d, format: %d", info.width, info.height, info.format);

    uint8_t* temp;
    result = AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&temp));

    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        MLOGE("AndroidBitmap_lockPixels failed, result: %d", result);
        return nullptr;
    }

    uint8_t* image_buffer = (uint8_t*)malloc(info.width * info.height * 4);
    if (!image_buffer) {
        MLOGE("ParseImageData alloc buffer failed");
        return nullptr;
    }
    // flip image data
    uint32_t stride = info.width * 4;
    for (int i = 0; i < info.height; i++) {
        memcpy(image_buffer + i * stride, temp + (info.height - 1 - i) * stride, stride);
    }

    AndroidBitmap_unlockPixels(env, bitmap);

    ImageData* data = new ImageData();
    data->width = info.width;
    data->height = info.height;
    data->data = image_buffer;
    data->premultiply_alpha = true;

    env->DeleteLocalRef(jarr);
    return data;
}

void PlayerEventEmitter::OnEvent(int player_id, int type, std::string msg) {
    MLOGD("PlayerEventEmitter OnEvent %d %d %s", player_id, type, msg.c_str());
    JNIEnv* env = JNIUtil::GetEnv();
    jclass clazz = JNIUtil::GetEventEmiterClass();
    if (!clazz) {
        MLOGE("PlayerEventEmitter jclass not found");
        return;
    }
    jmethodID mid = JNIUtil::GetEventEmiterOnEventMID();
    if (!mid) {
        MLOGE("PlayerEventEmitter jmethodID not found");
        return;
    }
    jstring jstr = env->NewStringUTF(msg.c_str());
    env->CallStaticVoidMethod(clazz, mid, player_id, type, jstr);
    env->DeleteLocalRef(jstr);
}

void LogUtil::platform_log(int level, const char* msg) {
    JNIEnv* env = JNIUtil::GetEnv();
    jclass clazz = JNIUtil::GetEventEmiterClass();
    if (!clazz) {
        return;
    }
    jmethodID mid = JNIUtil::GetEventEmiterOnLogMID();
    if (!mid) {
        return;
    }
    jstring jstr = env->NewStringUTF(msg);
    env->CallStaticVoidMethod(clazz, mid, level, jstr);
    env->DeleteLocalRef(jstr);
}

}
