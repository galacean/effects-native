package com.alipay.mobile.mars.player;

import static com.alipay.mobile.mars.util.Constants.ERROR_CREATE_MARS_DATA_FAIL;
import static com.alipay.mobile.mars.util.Constants.ERROR_CREATE_PLAYER_FAIL;
import static com.alipay.mobile.mars.util.Constants.ERROR_DOWNGRADE;
import static com.alipay.mobile.mars.util.Constants.ERROR_LOAD_BITMAP_FAIL;
import static com.alipay.mobile.mars.util.Constants.ERROR_LOAD_ZIP_FAIL;

import android.content.Context;
import android.graphics.Bitmap;
import android.support.annotation.UiThread;

import com.alipay.mobile.mars.adapter.MonitorUtil;
import com.alipay.mobile.mars.adapter.TaskScheduleUtil;
import com.alipay.mobile.mars.player.data.MarsDataBase;
import com.alipay.mobile.mars.util.CommonUtil;
import com.alipay.mobile.mars.adapter.LogUtil;

import java.lang.ref.WeakReference;
import java.util.Map;

public class MarsNativeBuilder {

    private final static String TAG = "MarsNativeBuilder";

    private final String mSource; // 业务名，用于埋点

    private final String mUrl; // 资源url 或 djangoId 或 本地路径目录 或 包内路径目录

    public String placeHolderUrl; // 降级图url 或 djangoId 或 本地路径 或 包内路径

    public Bitmap placeHolderBitmap; // 降级图bitmap

    public String scene; // 业务场景码，用于针对某个业务场景单独降级

    public Map<String, String> variables; // 可变参数表

    public Map<String, Bitmap> variablesBitmap; // 可变参数表，用于传Bitmap

    public int repeatCount = 0; // 默认为0，只播放1次；小于0则循环播放

    public long validTimestamp = -1;

    public String md5;

    public boolean fixTickTime = true;

    public boolean showPlaceHolderFirst = true;

    private MarsNativePlayerImpl mPlayer;

    private boolean mInitialized = false;

    private boolean mInitCallbackCalled = false;

    public MarsNativeBuilder(String source, String url) {
        mSource = source;
        mUrl = url;
    }

    // 创建player
    @UiThread
    public MarsNativePlayer build(Context context) throws Exception {
        if (mPlayer == null) {
            if (!CommonUtil.isMainThread()) {
                LogUtil.error(TAG, "build:sub thread");
                throw new Exception("build:sub thread");
            }
            mPlayer = new MarsNativePlayerImpl(context, this);
            LogUtil.debug(TAG, "build player success " + mPlayer + " " + mUrl);
        }
        return mPlayer;
    }

    @UiThread
    public void initPlayer(final InitCallback callback) throws Exception {
        if (callback == null) {
            LogUtil.error(TAG, "initPlayer:callback is null");
            throw new Exception("initPlayer:callback is null");
        }
        if (mInitialized) {
            LogUtil.error(TAG, "initPlayer:duplicate init");
            throw new Exception("initPlayer:duplicate init");
        }
        mInitialized = true;
        if (mPlayer == null) {
            LogUtil.error(TAG, "initPlayer:player is null");
            throw new Exception("initPlayer:player is null");
        }
        if (!CommonUtil.isMainThread()) {
            LogUtil.error(TAG, "initPlayer:sub thread");
            throw new Exception("initPlayer:sub thread");
        }

        final WeakReference<MarsNativePlayerImpl> weakPlayer = new WeakReference<>(mPlayer);
        TaskScheduleUtil.postToNormalThread(new Runnable() {
            @Override
            public void run() {
                // 1. 先检查placeHolder
                try {
                    MarsNativePlayerImpl player = weakPlayer.get();
                    if (player == null) {
                        LogUtil.error(TAG, "player is null on normal thread");
                        return;
                    }

                    PlaceHolderUtil.loadPlaceHolder(
                            player.getContext(), placeHolderUrl, placeHolderBitmap, mSource,
                            new MarsNativeResourceLoader.LoadBitmapCallback() {
                                @Override
                                public void onResult(Bitmap bitmap, String errMsg) {
                                    MarsNativePlayerImpl player = weakPlayer.get();
                                    if (player == null) {
                                        LogUtil.error(TAG, "player is null in placeHolder callback");
                                        return;
                                    }
                                    placeHolderBitmap = bitmap;
                                    if (placeHolderBitmap != null) {
                                        player.setPlaceHolder(placeHolderBitmap);
                                        callback.onShowPlaceHolder();
                                    }
                                    if (errMsg != null) {
                                        notifyFailEvent(ERROR_LOAD_BITMAP_FAIL, errMsg, callback, weakPlayer);
                                    }
                                }
                            });

                    // 2. 检查是否降级
                    String downgradeReason = player.checkDowngrade();
                    if (downgradeReason != null) {
                        notifyFailEvent(ERROR_DOWNGRADE, "downgrade:" + downgradeReason, callback, weakPlayer);
                        return;
                    }
                } catch (Exception e) {
                    LogUtil.error(TAG, "loadPlaceHolder..e:" + e.getMessage());
                    notifyFailEvent(ERROR_LOAD_BITMAP_FAIL, e.getMessage(), callback, weakPlayer);
                    return;
                }

                // 3. 加载动画资源
                String localDir = MarsNativeResourceLoader.loadFilePathFromLocal(mUrl);
                if (localDir != null) {
                    loadMarsDataFromDir(localDir, callback, weakPlayer);
                    return;
                }
                MarsNativeResourceLoader.loadZip(mUrl, mSource, validTimestamp, md5, new MarsNativeResourceLoader.LoadZipCallback() {
                    @Override
                    public void onSuccess(String dirPath) {
                        loadMarsDataFromDir(dirPath, callback, weakPlayer);
                    }

                    @Override
                    public void onError(String errMsg) {
                        LogUtil.error(TAG, "loadZip..e:" + errMsg);
                        notifyFailEvent(ERROR_LOAD_ZIP_FAIL, errMsg, callback, weakPlayer);
                    }
                });
            }
        });

        LogUtil.debug(TAG, "initPlayer running");
    }

    String getUrl() {
        return mUrl;
    }

    String getSource() {
        return mSource;
    }

    private void loadMarsDataFromDir(String dirPath, final InitCallback callback, final WeakReference<MarsNativePlayerImpl> weakPlayer) {
        LogUtil.debug(TAG, "loadZip success:" + dirPath);
        MarsNativePlayerImpl player = weakPlayer.get();
        if (player == null) {
            return;
        }
        MarsDataBase marsData = null;
        try {
            marsData = MarsDataBase.createMarsData(dirPath);
        } catch (Exception e) {
            notifyFailEvent(ERROR_CREATE_MARS_DATA_FAIL, e.getMessage(), callback, weakPlayer);
            return;
        }
        if (marsData == null) {
            notifyFailEvent(ERROR_CREATE_MARS_DATA_FAIL, "create mars data fail", callback, weakPlayer);
            return;
        }
        player.initWithMarsData(marsData, new MarsNativePlayerImpl.InitCallback() {
            @Override
            public void onResult(boolean success, String errMsg) {
                if (success) {
                    notifySuccessEvent(callback, weakPlayer);
                } else {
                    notifyFailEvent(ERROR_CREATE_PLAYER_FAIL, errMsg, callback, weakPlayer);
                }
            }
        });
    }

    private void notifySuccessEvent(final InitCallback callback, final WeakReference<MarsNativePlayerImpl> weakPlayer) {
        TaskScheduleUtil.postToUiThreadDelayed(new Runnable() {
            @Override
            public void run() {
                if (mInitCallbackCalled) {
                    LogUtil.debug(TAG, "notifySuccessEvent:duplicate");
                    return;
                }
                mInitCallbackCalled = true;

                LogUtil.debug(TAG, "notifySuccessEvent " + mPlayer);
                MonitorUtil.monitorStartEvent(mPlayer.getSourceId(), false, mSource, true);
                callback.onInitResult(true, 0, null);
            }
        }, 0);
    }

    private void notifyFailEvent(final int errCode, final String errMsg, final InitCallback callback, final WeakReference<MarsNativePlayerImpl> weakPlayer) {
        TaskScheduleUtil.postToUiThreadDelayed(new Runnable() {
            @Override
            public void run() {
                if (mInitCallbackCalled) {
                    LogUtil.debug(TAG, "notifyFailEvent:duplicate");
                    return;
                }
                mInitCallbackCalled = true;

                LogUtil.error(TAG, "notifyFailEvent..e:" + mPlayer + " " + errCode + "," + errMsg);
                MonitorUtil.monitorErrorEvent(
                        MarsNativePlayerImpl.getFormattedSourceId(mUrl), "init_error", errMsg
                );
                callback.onInitResult(false, errCode, errMsg);
                MarsNativePlayerImpl player = weakPlayer.get();
                if (player != null) {
                    player.setDowngrade();
                }
            }
        }, 0);
    }

    public static abstract class InitCallback {
        protected void onShowPlaceHolder() {}

        protected abstract void onInitResult(boolean success, int errCode, String errMsg);
    }
}
