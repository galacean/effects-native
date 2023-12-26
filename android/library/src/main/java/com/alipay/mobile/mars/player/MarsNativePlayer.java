package com.alipay.mobile.mars.player;

import android.content.Context;
import android.graphics.Bitmap;
import android.support.annotation.NonNull;
import android.widget.FrameLayout;

public abstract class MarsNativePlayer extends FrameLayout {
    public MarsNativePlayer(@NonNull Context context) {
        super(context);
    }

    public abstract void play();

    public abstract void play(MarsNativePlayerImpl.PlayCallback callback);

    public abstract void play(int repeatCount, MarsNativePlayerImpl.PlayCallback callback);

    public abstract void play(int fromFrame, int toFrame, int repeatCount, MarsNativePlayerImpl.PlayCallback callback);

    public abstract void stop();

    public abstract void pause();

    public abstract void resume();

    public abstract void destroy();

    public abstract float getAspect();

    public abstract PreviewSize getPreviewSize();

    public abstract float getDuration();

    public abstract void setEventListener(EventListener listener);

    public abstract void addExtension(MarsNativeExtension extension);

    // 首帧渲染，只有第一次调用play时会收到
    public abstract void onFirstScreen(Runnable runnable);

    public abstract boolean updateVariable(String key, Bitmap bitmap);

    public static abstract class PlayCallback {
        protected abstract void onPlayFinish();

        protected void onRuntimeError(String errMsg) {}
    }

    public interface EventListener {
        void onMessageItem(String itemName, String phrase);
    }

    public interface MarsNativeExtension {
        boolean onSceneDataCreated(long sceneDataPtr, String[] errMsg);

        long getCustomPlugin(String[] name);

        void onDestroy();
    }

    public static class PreviewSize {
        public float width;
        public float height;
        public PreviewSize(float width, float height) {
            this.width = width;
            this.height = height;
        }
    }
}

