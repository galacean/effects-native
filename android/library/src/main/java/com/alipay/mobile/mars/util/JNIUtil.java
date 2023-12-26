package com.alipay.mobile.mars.util;

import android.graphics.Bitmap;
import android.support.annotation.Keep;
import android.view.Surface;

import java.util.ArrayList;

public class JNIUtil {

    @Keep
    public static abstract class CommonJNICallback {
        @Keep
        public abstract void onCallback(String[] params);
    }

    public static native void nativeMarsCreate(int idx, int renderLevel, boolean enableSurfaceScale, boolean fixTickTime);

    public static native void nativeMarsAddPlugin(int idx, long ptr, String name);

    public static native void nativeSetRepeatCount(int idx, int repeatCount);

    public static native boolean nativeSetupSurface(int idx, Surface surface);

    public static native void nativeResizeSurface(int idx, int width, int height);

    public static native void nativeDestroySurface(int idx);

    public static native void nativeSetSceneData(int idx, long sceneDataPtr);

    /**
     *
     * @param idx playerId
     * @param playToken 本次播放的token，标记对应的上层回调是哪一个
     * @param needLock Android在play时锁一下，防止多线程同时创建shader，解决部分机型闪退
     */
    public static native void nativeMarsPlay(int idx, String playToken, boolean needLock);

    public static native void nativeMarsStop(int idx);

    /**
     *
     * @param idx playerId
     * @param fromFrame 起始帧
     * @param toFrame 结束帧
     * @param playToken 本次播放的token，标记对应的上层回调是哪一个
     * @param needLock Android在play时锁一下，防止多线程同时创建shader，解决部分机型闪退
     */
    public static native void nativeMarsPlayFrameControl(int idx, int fromFrame, int toFrame, String playToken, boolean needLock);

    public static native void nativeMarsDestroy(int idx);

    /**
     * 没参数的jni都走这个，减少jni方法
     * @param idx player id
     * @param type 1：pause，2：resume
     */
    public static native void nativeMarsEvent(int idx, int type);

    public static native long nativeSceneDataCreateByPath(String filePath);

    public static native long nativeSceneDataCreate(byte[] sceneData, int len);

    public static native long nativeSceneDataCreateBySingleBinPath(String filePath);

    public static native long nativeSceneDataCreateBySingleBinData(byte[] sceneData, int len);

    public static native void nativeSceneDataDestroy(long sceneDataPtr);

    // 作用是更改composition的endBehavior
    public static native void nativeSceneDataSetLooping(long ptr, boolean looping);

    public static native void nativeSetSceneImageResource(long ptr, String url, byte[] imageData, int len);

    // bitmap是alpha预乘过的
    public static native boolean nativeSetSceneImageResourceBmp(long ptr, String url, Bitmap bitmap);

    // 视频纹理
    public static native void nativeSetSceneImageResourceVideo(long ptr, String url, long vcPtr);

    public static native float nativeSceneDataGetAspect(long ptr);

    public static native float nativeSceneDataGetDuration(long ptr);

    // ResourceLoader
    public static native long nativeCreateVideoContextAndPrepare(
            int playerIdx, String filePath, String key, boolean transparent, boolean hardDecoder,
            CommonJNICallback callback);

    public static native boolean nativeUpdateMarsImage(int playerIdx, int templateIdx, Bitmap bitmap);

    static {
        System.loadLibrary("marsnative");
    }
}
