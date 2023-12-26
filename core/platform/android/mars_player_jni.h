#ifndef MN_MARS_PLAYER_JNI_H_
#define MN_MARS_PLAYER_JNI_H_

#include <jni.h>

namespace mn {

class MarsPlayerJNI {
public:
    static void CreateNew(JNIEnv *env, jclass clazz, jint idx, jint render_level, jboolean enable_surface_scale, jboolean fix_tick_time);

    static void AddPlugin(JNIEnv *env, jclass clazz, jint idx, jlong ptr, jstring jname);

    static void SetRepeatCount(JNIEnv *env, jclass clazz, jint idx, jint repeat_count);

    static jboolean SetupSurface(JNIEnv *env, jclass clazz, jint idx, jobject surface);

    static void ResizeSurface(JNIEnv *env, jclass clazz, jint idx, jint width, jint height);

    static void DestroySurface(JNIEnv *env, jclass clazz, jint idx);

    static void SetSceneData(JNIEnv *env, jclass clazz, jint idx, jlong scene_data_ptr);

    static void PlayMars(JNIEnv *env, jclass clazz, jint idx, jstring play_token, jboolean need_lock);

    static void PlayMarsFrameControl(JNIEnv *env, jclass clazz, jint idx, jint from_frame, jint to_frame, jstring play_token, jboolean need_lock);

    static void StopMars(JNIEnv *env, jclass clazz, jint idx);

    static void DestroyMars(JNIEnv *env, jclass clazz, jint idx);

    static void OnEvent(JNIEnv *env, jclass clazz, jint idx, jint type);

    static jlong CreateSceneData(JNIEnv *env, jclass clazz, jbyteArray scene_data, jint len);

    static jlong CreateSceneDataByPath(JNIEnv *env, jclass clazz, jstring scene_data_path);

    static jlong CreateSceneDataByBinData(JNIEnv *env, jclass clazz, jbyteArray scene_data, jint len);

    static jlong CreateSceneDataByBinPath(JNIEnv *env, jclass clazz, jstring scene_data_path);

    static void DestroySceneData(JNIEnv *env, jclass clazz, jlong scene_data_ptr);

    static void SceneDataSetLooping(JNIEnv *env, jclass clazz, jlong scene_data_ptr, jboolean looping);

    static void SceneDataSetImageData(JNIEnv *env, jclass clazz, jlong scene_data_ptr, jstring url, jbyteArray scene_data, jint len);

    static jboolean SceneDataSetBitmapData(JNIEnv *env, jclass clazz, jlong scene_data_ptr, jstring url, jobject bitmap);

    static void SceneDataSetVideoData(JNIEnv *env, jclass clazz, jlong scene_data_ptr, jstring url, jlong vc_ptr);

    static float SceneDataGetAspect(JNIEnv *env, jclass clazz, jlong scene_data_ptr);

};

}

#endif
