package com.antgroup.galacean.effects;

import android.content.Context;
import android.text.TextUtils;
import android.view.View;

import com.alipay.mobile.mars.adapter.ContextUtil;
import com.alipay.mobile.mars.adapter.TaskScheduleUtil;
import com.alipay.mobile.mars.player.MarsNativeBuilder;
import com.alipay.mobile.mars.player.MarsNativePlayer;
import com.alipay.mobile.mars.adapter.LogUtil;

import java.lang.ref.WeakReference;

class GEPlayerImpl {
    private final static String TAG = "GEPlayerImpl";

    private GEPlayer.GEPlayerParams mParams;

    private MarsNativePlayer mPlayer = null;

    GEPlayerImpl(GEPlayer.GEPlayerParams params) {
        mParams = params;
    }

    View getView() {
        return mPlayer;
    }

    void loadScene(Context context, GEPlayer.Callback callback) {
        if (mPlayer != null) {
            TaskScheduleUtil.postToUiThreadDelayed(new Runnable() {
                @Override
                public void run() {
                    if (callback != null) {
                        callback.onResult(false, "loadScene duplicated");
                    }
                }
            }, 0);
            return;
        }
        if (mParams == null || TextUtils.isEmpty(mParams.url)) {
            TaskScheduleUtil.postToUiThreadDelayed(new Runnable() {
                @Override
                public void run() {
                    if (callback != null) {
                        callback.onResult(false, "url is null");
                    }
                }
            }, 0);
            return;
        }
        try {
            ContextUtil.setApplicationContext(context.getApplicationContext());
            MarsNativeBuilder builder = new MarsNativeBuilder("galacean", mParams.url);
            builder.placeHolderBitmap = mParams.downgradeImage;
            builder.variables = mParams.variables;
            builder.variablesBitmap = mParams.variablesBitmap;
            builder.showPlaceHolderFirst = false;
            mPlayer = builder.build(context);
            WeakReference<GEPlayerImpl> weakThiz = new WeakReference<>(this);
            builder.initPlayer(new MarsNativeBuilder.InitCallback() {
                @Override
                protected void onInitResult(boolean success, int errCode, String errMsg) {
                    if (callback == null) {
                        LogUtil.error(TAG, "onInitResult without callback");
                        return;
                    }
                    GEPlayerImpl thiz = weakThiz.get();
                    if (thiz == null || thiz.mPlayer == null) { // destroy之后不再回调
                        return;
                    }
                    if (success) {
                        callback.onResult(true, "");
                    } else {
                        callback.onResult(false, errMsg);
                    }
                }
            });
        } catch (Throwable e) {
            LogUtil.error(TAG, "loadScene.e:" + e.getMessage());
            TaskScheduleUtil.postToUiThreadDelayed(new Runnable() {
                @Override
                public void run() {
                    if (callback != null) {
                        callback.onResult(false, e.getMessage());
                    }
                }
            }, 0);
        }
    }

    void play(int repeatCount, GEPlayer.Callback callback) {
        if (mPlayer != null) {
            mPlayer.play(repeatCount, new PlayCallback(callback));
        }
    }

    void play(int fromFrame, int toFrame, int repeatCount, GEPlayer.Callback callback) {
        if (mPlayer != null) {
            mPlayer.play(fromFrame, toFrame, repeatCount, new PlayCallback(callback));
        }
    }

    void pause() {
        if (mPlayer != null) {
            mPlayer.pause();
        }
    }

    void resume() {
        if (mPlayer != null) {
            mPlayer.resume();
        }
    }

    void stop() {
        if (mPlayer != null) {
            mPlayer.stop();
        }
    }

    void destroy() {
        if (mPlayer != null) {
            mPlayer.destroy();
            mPlayer = null;
        }
    }

    float getAspect() {
        if (mPlayer != null) {
            return mPlayer.getAspect();
        }
        return -1;
    }

    int getFrameCount() {
        if (mPlayer != null) {
            return (int) Math.floor(mPlayer.getDuration() * 1000.0 / 33.0);
        }
        return -1;
    }

    private static class PlayCallback extends MarsNativePlayer.PlayCallback {
        private GEPlayer.Callback mCallback;

        PlayCallback(GEPlayer.Callback callback) {
            mCallback = callback;
        }

        @Override
        protected void onPlayFinish() {
            try {
                if (mCallback != null) {
                    mCallback.onResult(true, "");
                }
            } catch (Throwable e) {
                LogUtil.error(TAG, "onPlayFinish.e:" + e.getMessage());
            }
        }

        @Override
        protected void onRuntimeError(String errMsg) {
            try {
                if (mCallback != null) {
                    mCallback.onResult(false, errMsg);
                }
            } catch (Throwable e) {
                LogUtil.error(TAG, "onRuntimeError.e:" + e.getMessage());
            }
        }
    }
}
