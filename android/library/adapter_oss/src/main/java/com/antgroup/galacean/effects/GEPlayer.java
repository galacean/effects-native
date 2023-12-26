package com.antgroup.galacean.effects;

import android.content.Context;
import android.graphics.Bitmap;
import android.support.annotation.NonNull;
import android.view.View;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import java.util.Map;

public class GEPlayer extends FrameLayout {

    private final GEPlayerImpl mImpl;

    public GEPlayer(@NonNull Context context, GEPlayerParams params) {
        super(context);

        mImpl = new GEPlayerImpl(params);
    }

    public void loadScene(Callback callback) {
        if (mImpl != null) {
            mImpl.loadScene(getContext(), callback);
            View view = mImpl.getView();
            if (view != null) {
                addView(view, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
            }
        }
    }

    public void play(int repeatCount, GEPlayer.Callback callback) {
        if (mImpl != null) {
            mImpl.play(repeatCount, callback);
        }
    }

    public void play(int fromFrame, int toFrame, int repeatCount, GEPlayer.Callback callback) {
        if (mImpl != null) {
            mImpl.play(fromFrame, toFrame, repeatCount, callback);
        }
    }

    public void pause() {
        if (mImpl != null) {
            mImpl.pause();
        }
    }

    public void resume() {
        if (mImpl != null) {
            mImpl.resume();
        }
    }

    public void stop() {
        if (mImpl != null) {
            mImpl.stop();
        }
    }

    public void destroy() {
        if (mImpl != null) {
            mImpl.destroy();
        }
    }

    public float getAspect() {
        if (mImpl != null) {
            return mImpl.getAspect();
        }
        return -1;
    }

    public int getFrameCount() {
        if (mImpl != null) {
            return mImpl.getFrameCount();
        }
        return -1;
    }

    public static class GEPlayerParams {
        public String url;
        public Bitmap downgradeImage;
        public Map<String, String> variables;
        public Map<String, Bitmap> variablesBitmap;
    }

    public interface Callback {
        void onResult(boolean success, String errorMsg);
    }
}
