#include "mars_player_jni.h"
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/bitmap.h>
#include "player/mars_player_mgr.h"
#include "platform/android/mars_surface_android.h"
#include "platform/android/jni_bridge.h"
#include "sceneData/scene_data_decoder.h"
#include "sceneData/scene_data_decoder_new.hpp"
#include "util/log_util.hpp"
#include "video/video_image_data.hpp"

namespace mn {

void MarsPlayerJNI::CreateNew(JNIEnv *env, jclass clazz, jint idx, jint render_level, jboolean enable_surface_scale, jboolean fix_tick_time) {
    MarsPlayerConstructor options;
    
    bool surface_scale_enable = enable_surface_scale == JNI_TRUE;
    options.surface = new MarsSurfaceAndroid(surface_scale_enable);
    // todo: 上层接口层传进来
    options.auto_tick = true;
    options.config.render_level = render_level;
    options.config.fix_tick_time = fix_tick_time == JNI_TRUE;
    
    MarsPlayerManager::CreateNew(idx, options, true);
}

void MarsPlayerJNI::AddPlugin(JNIEnv *env, jclass clazz, jint idx, jlong ptr, jstring jname) {
    const char* name = env->GetStringUTFChars(jname, NULL);
    MarsPlayerManager::AddPlugin(idx, (void*) ptr, name);
    env->ReleaseStringUTFChars(jname, name);
}

void MarsPlayerJNI::SetRepeatCount(JNIEnv *env, jclass clazz, jint idx, jint repeat_count) {
    MarsPlayerManager::SetRepeatCount(idx, repeat_count);
}

jboolean MarsPlayerJNI::SetupSurface(JNIEnv *env, jclass clazz, jint idx, jobject surface) {
    ANativeWindow* native_window = ANativeWindow_fromSurface(env, surface);
    MarsPlayerManager::OnSurfaceCreate(idx, native_window);
    return JNI_TRUE;
}

void MarsPlayerJNI::ResizeSurface(JNIEnv *env, jclass clazz, jint idx, jint width, jint height) {
    MarsPlayerManager::OnSurfaceResize(idx, width, height);
}

void MarsPlayerJNI::DestroySurface(JNIEnv *env, jclass clazz, jint idx) {
    MarsPlayerManager::OnSurfaceDestroy(idx);
}

void MarsPlayerJNI::SetSceneData(JNIEnv *env, jclass clazz, jint idx, jlong scene_data_ptr) {
    MarsPlayerManager::SetSceneData(idx, (SceneDataDecoder*) scene_data_ptr);
}

void MarsPlayerJNI::PlayMars(JNIEnv *env, jclass clazz, jint idx, jstring play_token, jboolean need_lock) {
    const char* token = env->GetStringUTFChars(play_token, NULL);
    MarsPlayerManager::PlayMars(idx, token, need_lock);
    env->ReleaseStringUTFChars(play_token, token);
}

void MarsPlayerJNI::PlayMarsFrameControl(JNIEnv *env, jclass clazz, jint idx, jint from_frame, jint to_frame, jstring play_token, jboolean need_lock) {
    const char* token = env->GetStringUTFChars(play_token, NULL);
    MarsPlayerManager::PlayMarsFrameControl(idx, from_frame, to_frame, token, need_lock);
    env->ReleaseStringUTFChars(play_token, token);
}

void MarsPlayerJNI::StopMars(JNIEnv *env, jclass clazz, jint idx) {
    MarsPlayerManager::StopMars(idx);
}

void MarsPlayerJNI::DestroyMars(JNIEnv *env, jclass clazz, jint idx) {
    MarsPlayerManager::DestroyMars(idx);
}

void MarsPlayerJNI::OnEvent(JNIEnv *env, jclass clazz, jint idx, jint type) {
    MarsPlayerManager::OnEvent(idx, type);
}

jlong MarsPlayerJNI::CreateSceneData(JNIEnv *env, jclass clazz, jbyteArray scene_data, jint len) {
    uint8_t* buffer = (uint8_t*)malloc(len);
    if (!buffer) {
        return 0;
    }
    env->GetByteArrayRegion(scene_data, 0, len, (jbyte*)buffer);

    return (jlong) SceneDataDecoderNew::CreateNew(buffer);
}

jlong MarsPlayerJNI::CreateSceneDataByPath(JNIEnv *env, jclass clazz, jstring scene_data_path) {
    const char* path = env->GetStringUTFChars(scene_data_path, NULL);
    jlong ret = (jlong) SceneDataDecoderNew::CreateNewByPath(path);
    env->ReleaseStringUTFChars(scene_data_path, path);
    return ret;
}

jlong MarsPlayerJNI::CreateSceneDataByBinData(JNIEnv *env, jclass clazz, jbyteArray scene_data, jint len) {
    uint8_t* buffer = (uint8_t*)malloc(len);
    if (!buffer) {
        return 0;
    }
    env->GetByteArrayRegion(scene_data, 0, len, (jbyte*)buffer);

    return (jlong) SceneDataDecoderNew::CreateNewBySingleBinData(buffer, len);
}

jlong MarsPlayerJNI::CreateSceneDataByBinPath(JNIEnv *env, jclass clazz, jstring scene_data_path) {
    const char* path = env->GetStringUTFChars(scene_data_path, NULL);
    jlong ret = (jlong) SceneDataDecoderNew::CreateNewBySingleBinPath(path);
    env->ReleaseStringUTFChars(scene_data_path, path);
    return ret;
}

void MarsPlayerJNI::DestroySceneData(JNIEnv *env, jclass clazz, jlong scene_data_ptr) {
    if (scene_data_ptr == 0 || scene_data_ptr == -1) {
        return;
    }
    delete (SceneDataDecoder*) scene_data_ptr;
}

void MarsPlayerJNI::SceneDataSetImageData(JNIEnv *env, jclass clazz, jlong scene_data_ptr, jstring url, jbyteArray image_data, jint len) {
    const char* image_url = env->GetStringUTFChars(url, NULL);

    MLOGD("setImageData: %s length %d", image_url, len);
    uint8_t* image_buffer = (uint8_t*)malloc(len);
    env->GetByteArrayRegion(image_data, 0, len, (jbyte*)image_buffer);
    
    SceneDataDecoder* scene_data_decoder = (SceneDataDecoder*) scene_data_ptr;
    ImageRawData* image_raw_data = new ImageRawData(image_buffer, len);
    scene_data_decoder->SetImageData(image_url, image_raw_data);
    env->ReleaseStringUTFChars(url, image_url);
}

jboolean MarsPlayerJNI::SceneDataSetBitmapData(JNIEnv *env, jclass clazz, jlong scene_data_ptr, jstring url, jobject bitmap) {
    if (bitmap == nullptr) {
        MLOGE("SceneDataSetBitmapData empty bitmap");
        return JNI_FALSE;
    }
    const char* image_url = env->GetStringUTFChars(url, NULL);

    AndroidBitmapInfo info;
    int result;

    result = AndroidBitmap_getInfo(env, bitmap, &info);
    if (result != ANDROID_BITMAP_RESUT_SUCCESS || info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        MLOGE("AndroidBitmap_getInfo failed, result: %d, format: %d", result, info.format);
        return JNI_FALSE;
    }

    MLOGD("setImageData: %s width: %d, height: %d, format: %d",
        image_url, info.width, info.height, info.format);

    uint8_t* temp;
    result = AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&temp));

    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        MLOGE("AndroidBitmap_lockPixels failed, result: %d", result);
        return JNI_FALSE;
    }

    uint8_t* image_buffer = (uint8_t*)malloc(info.width * info.height * 4);
    if (!image_buffer) {
        MLOGE("setImageData alloc buffer failed");
        return JNI_FALSE;
    }
    // flip image data
    uint32_t stride = info.width * 4;
    for (int i = 0; i < info.height; i++) {
        memcpy(image_buffer + i * stride, temp + (info.height - 1 - i) * stride, stride);
    }

    AndroidBitmap_unlockPixels(env, bitmap);
    
    SceneDataDecoder* scene_data_decoder = (SceneDataDecoder*) scene_data_ptr;
    ImageRawData* image_raw_data = new ImageRawData(image_buffer, info.width, info.height);
    image_raw_data->premultiply_alpha = true;
    scene_data_decoder->SetImageData(image_url, image_raw_data);
    env->ReleaseStringUTFChars(url, image_url);

    return JNI_TRUE;
}

void MarsPlayerJNI::SceneDataSetVideoData(JNIEnv *env, jclass clazz, jlong scene_data_ptr, jstring url, jlong vc_ptr) {
    const char* image_url = env->GetStringUTFChars(url, NULL);

    auto scene_data_decoder = (SceneDataDecoder*) scene_data_ptr;
    auto vc = (VideoContext*)vc_ptr;
    MLOGD("setVideoData: %s length %p", image_url, vc);
    auto video_data = new VideoImageRawData(vc);
    scene_data_decoder->SetImageData(image_url, video_data);

    env->ReleaseStringUTFChars(url, image_url);
}

void MarsPlayerJNI::SceneDataSetLooping(JNIEnv *env, jclass clazz, jlong scene_data_ptr, jboolean looping) {
    SceneDataDecoder* scene_data_decoder = (SceneDataDecoder *)scene_data_ptr;
    scene_data_decoder->SetAnimationLoop(looping);
}

float MarsPlayerJNI::SceneDataGetAspect(JNIEnv *env, jclass clazz, jlong scene_data_ptr) {
    SceneDataDecoder* scene_data_decoder = (SceneDataDecoder *)scene_data_ptr;
    if (scene_data_decoder && scene_data_decoder->compositions_.size() > 0) {
        auto& composition = scene_data_decoder->compositions_[0];
        if (composition && composition->preview_size) {
            return (composition->preview_size[0] / composition->preview_size[1]);
        }
    }
    return 1;
}

}

extern "C" JNIEXPORT jfloat JNICALL
Java_com_alipay_mobile_mars_util_JNIUtil_nativeSceneDataGetDuration(
        JNIEnv *env, jclass clazz, jlong scene_data_ptr) {
    mn::SceneDataDecoder* scene_data_decoder = (mn::SceneDataDecoder *)scene_data_ptr;
    if (scene_data_decoder && (!scene_data_decoder->compositions_.empty())) {
        auto& composition = scene_data_decoder->compositions_[0];
        if (composition) {
            return composition->duration;
        }
    }
    return 0;
}

extern "C"
JNIEXPORT jboolean JNICALL
Java_com_alipay_mobile_mars_util_JNIUtil_nativeUpdateMarsImage(JNIEnv *env, jclass clazz,
                                                               jint player_idx, jint template_idx,
                                                               jobject bitmap) {
    if (bitmap == nullptr) {
        MLOGE("UpdateMarsImage empty bitmap");
        return JNI_FALSE;
    }

    AndroidBitmapInfo info;
    int result = AndroidBitmap_getInfo(env, bitmap, &info);
    if (result != ANDROID_BITMAP_RESUT_SUCCESS || info.format != ANDROID_BITMAP_FORMAT_RGBA_8888) {
        MLOGE("AndroidBitmap_getInfo failed, result: %d, format: %d", result, info.format);
        return JNI_FALSE;
    }

    uint8_t* temp;
    result = AndroidBitmap_lockPixels(env, bitmap, reinterpret_cast<void **>(&temp));
    if (result != ANDROID_BITMAP_RESULT_SUCCESS) {
        MLOGE("AndroidBitmap_lockPixels failed, result: %d", result);
        return JNI_FALSE;
    }

    uint8_t* image_buffer = (uint8_t*)malloc(info.width * info.height * 4);
    if (!image_buffer) {
        MLOGE("UpdateMarsImage alloc buffer failed");
        return JNI_FALSE;
    }

    // flip image data
    uint32_t stride = info.width * 4;
    for (int i = 0; i < info.height; i++) {
        memcpy(image_buffer + i * stride, temp + (info.height - 1 - i) * stride, stride);
    }

    AndroidBitmap_unlockPixels(env, bitmap);

    ImageRawData* image_raw_data = new ImageRawData(image_buffer, info.width, info.height);
    image_raw_data->premultiply_alpha = true;
    bool success = mn::MarsPlayerManager::UpdateMarsImage(player_idx, template_idx, image_raw_data);
    return success ? JNI_TRUE : JNI_FALSE;
}