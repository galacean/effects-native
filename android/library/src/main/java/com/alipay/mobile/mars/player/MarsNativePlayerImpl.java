package com.alipay.mobile.mars.player;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Point;
import android.text.TextUtils;
import android.view.Surface;
import android.view.ViewGroup;
import android.widget.ImageView;

import com.alipay.mobile.mars.adapter.DowngradeUtil;
import com.alipay.mobile.mars.EventEmitter;
import com.alipay.mobile.mars.adapter.ConfigUtil;
import com.alipay.mobile.mars.adapter.MonitorUtil;
import com.alipay.mobile.mars.adapter.TaskScheduleUtil;
import com.alipay.mobile.mars.player.data.MarsDataBase;
import com.alipay.mobile.mars.player.data.MarsDataJsonBin;
import com.alipay.mobile.mars.util.Constants;
import com.alipay.mobile.mars.util.JNIUtil;
import com.alipay.mobile.mars.adapter.LogUtil;
import com.alipay.mobile.mars.view.MarsTextureView;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.concurrent.atomic.AtomicInteger;

public class MarsNativePlayerImpl extends MarsNativePlayer implements EventEmitter.EventListener, MarsTextureView.MarsSurfaceListener {

    private final static String TAG = "MarsNativePlayer";

    private final String mSource;
    // formatted url
    private final String mUrl;
    private final String mScene;
    private int mDeviceLevel = Constants.DEVICE_LEVEL_HIGH;
    private boolean mFixTickTime = true;

    private static boolean sIsLoadSo = false;
    private int mNativePlayer = -1;
    private long mSceneDataPtr = 0;
    private MarsTextureView mMarsView;

    private int mRepeatCount = -1;

    private int mFrameCount = 0;

    private boolean mDowngrade = false;

    private ImageView mPlaceHolder;
    private int mPlaceHolderVisibility;
    private float mAspect = 1.0f;
    private float mDuration = 0.0f;

    private MarsDataBase mMarsData = null;

    private Map<String, String> mVariables = null;

    private Map<String, Bitmap> mVariableBitmaps = null;

    private EventListener mEventListener = null;

    private final Object mPtrMutex = new Object();

    private InitCallback mInitCallback;
    private Runnable mFirstScreenCallback = null;
    private PlayCallbackHolder mPlayCallbackHolder;

    private ArrayList<MarsNativeExtension> mExtensions = new ArrayList<>();

    // 临时存储，防止GC
    private ArrayList<MarsNativeResourceLoader.MarsNativeImageResource> mResList;

    MarsNativePlayerImpl(Context context, MarsNativeBuilder builder) {
        super(context);
        mSource = builder.getSource();
        mUrl = getFormattedSourceId(builder.getUrl());
        mScene = builder.scene;
        mRepeatCount = builder.repeatCount;
        mVariables = builder.variables;
        mVariableBitmaps = builder.variablesBitmap;
        mFixTickTime = builder.fixTickTime;
        mPlaceHolderVisibility = builder.showPlaceHolderFirst ? VISIBLE : GONE;
    }

    /**
     * 检查是否降级
     * @return 降级原因，null表示不降级
     */
    String checkDowngrade() {
        String downgradeReason = null;
        // 先检查总降级开关
        boolean shouldDowngrade = ConfigUtil.forceDowngradeMN();
        downgradeReason = "forceDowngradeMN";
        if (!shouldDowngrade) {
            // 检查资源纬度降级开关
            if (TextUtils.isEmpty(mScene)) {
                // 没有场景码，按资源降级
                shouldDowngrade = ConfigUtil.forceDowngradeByResId(mUrl);
                downgradeReason = "forceDowngradeByResId," + mUrl;
            } else {
                // 有场景码，按场景降级
                shouldDowngrade = ConfigUtil.forceDowngradeByResScene(mScene);
                downgradeReason = "forceDowngradeByResScene," + mScene;
            }
        }
        if (!shouldDowngrade) {
            // 没有开关降级，检查统一降级
            DowngradeUtil.DowngradeResult downgradeResult = DowngradeUtil.getDowngradeResult(mUrl);
            if (downgradeResult.getDowngrade()) {
                // 统一降级降级了
                shouldDowngrade = true;
                downgradeReason = "getDowngradeResult," + downgradeResult.getReason();
            } else {
                // 没降级，获取设备等级
                mDeviceLevel = downgradeResult.getDeviceLevel();
                LogUtil.debug(TAG, "getDowngradeResult: deviceLevel " + mDeviceLevel);
            }
        }

        if (shouldDowngrade) {
            TaskScheduleUtil.postToUiThreadDelayed(new Runnable() {
                @Override
                public void run() {
                    setDowngrade();
                }
            }, 0);
            return downgradeReason;
        }
        return null;
    }

    @Override
    public void play() {
        play(0, mFrameCount, mRepeatCount, null);
    }

    @Override
    public void play(PlayCallback callback) {
        play(0, mFrameCount, mRepeatCount, callback);
    }

    @Override
    public void play(int repeatCount, PlayCallback callback) {
        play(0, mFrameCount, repeatCount, callback);
    }

    @Override
    public void play(int fromFrame, int toFrame, int repeatCount, final PlayCallback callback) {
        if (mDowngrade) {
            TaskScheduleUtil.postToUiThreadDelayed(new Runnable() {
                @Override
                public void run() {
                    if (callback != null) {
                        callback.onRuntimeError("downgrade");
                    }
                }
            }, 0);
            return;
        }
        LogUtil.debug(TAG, "play " + this);
        mRepeatCount = repeatCount;
        mPlayCallbackHolder = new PlayCallbackHolder(callback);
        JNIUtil.nativeSetRepeatCount(mNativePlayer, mRepeatCount);
        JNIUtil.nativeMarsPlayFrameControl(mNativePlayer, fromFrame, toFrame, mPlayCallbackHolder.mToken, true);
    }

    @Override
    public void stop() {
        if (mDowngrade) {
            return;
        }
        LogUtil.debug(TAG, "stop " + this);
        JNIUtil.nativeMarsStop(mNativePlayer);
    }

    @Override
    public void pause() {
        if (mDowngrade) {
            return;
        }
        LogUtil.debug(TAG, "pause " + this);
        JNIUtil.nativeMarsEvent(mNativePlayer, Constants.NATIVE_EVENT_PAUSE);
    }

    @Override
    public void resume() {
        if (mDowngrade) {
            return;
        }
        LogUtil.debug(TAG, "resume " + this);
        JNIUtil.nativeMarsEvent(mNativePlayer, Constants.NATIVE_EVENT_RESUME);
    }

    @Override
    public void destroy() {
        synchronized (mPtrMutex) {
            LogUtil.debug(TAG, "destroy " + mNativePlayer + " " + this);
            if (mNativePlayer != -1) {
                JNIUtil.nativeMarsDestroy(mNativePlayer);
                EventEmitter.unregisterListener(mNativePlayer);
                mNativePlayer = -1;
            }
            if (mSceneDataPtr != 0 && mSceneDataPtr != -1) {
                JNIUtil.nativeSceneDataDestroy(mSceneDataPtr);
                mSceneDataPtr = 0;
            }
            mMarsView = null;
            mInitCallback = null;
            mPlayCallbackHolder = null;

            for (MarsNativeExtension extension : mExtensions) {
                extension.onDestroy();
            }
            mExtensions.clear();
        }
    }

    @Override
    public float getAspect() {
        return mAspect;
    }

    public PreviewSize getPreviewSize() {
        Point tmp = mMarsData.getPreviewSize();
        return new PreviewSize((float) tmp.x, (float) tmp.y);
    }

    @Override
    public float getDuration() {
        return mDuration;
    }

    @Override
    public void setEventListener(EventListener listener) {
        mEventListener = listener;
    }

    @Override
    public void addExtension(MarsNativeExtension extension) {
        mExtensions.add(extension);
    }

    @Override
    public void onFirstScreen(Runnable runnable) {
        mFirstScreenCallback = runnable;
    }

    @Override
    public boolean updateVariable(String key, Bitmap bitmap) {
        if (bitmap == null) {
            LogUtil.error(TAG, "updateVariable: bitmap is null");
            return false;
        }

        List<MarsDataBase.ImageInfo> imgs =  mMarsData.getImages();
        int templateIdx = -1;
        for (MarsDataBase.ImageInfo img : imgs) {
            if (img.url.equals(key)) {
                templateIdx = img.templateIdx;
                break;
            }
        }

        if (templateIdx == -1) {
            LogUtil.error(TAG, "updateVariable: teamplateIdx is -1");
            return false;
        }

        return JNIUtil.nativeUpdateMarsImage(mNativePlayer, templateIdx, bitmap);
    }

    void initWithMarsData(MarsDataBase marsData, InitCallback callback) {
        LogUtil.debug(TAG, "initWithMarsData:type=" + marsData.getType());

        if (mDowngrade) {
            return;
        }

        mMarsData = marsData;

        synchronized (mPtrMutex) {
            mAspect = marsData.getAspect();
            mDuration = marsData.getDuration();
            mFrameCount = (int) (marsData.getDuration() * 30);
            mInitCallback = callback;

            if (!tryLoadSo()) {
                callback.onResult(false, "load so fail");
                return;
            }

            mNativePlayer = generatePlayerIndex();
            EventEmitter.registerListener(mNativePlayer, this);
            JNIUtil.nativeMarsCreate(mNativePlayer,
                    ConfigUtil.getRenderLevel(mDeviceLevel),
                    ConfigUtil.enableSurfaceScale(mUrl),
                    mFixTickTime);
            for (int i = 0; i < mExtensions.size(); i++) {
                String[] name = new String[1];
                long ptr = mExtensions.get(i).getCustomPlugin(name);
                if (ptr != 0) {
                    JNIUtil.nativeMarsAddPlugin(mNativePlayer, ptr, name[0]);
                }
            }
            if (mNativePlayer == -1) {
                callback.onResult(false, "create player fail");
                return;
            }
            tryLoadImages(marsData);
            JNIUtil.nativeSetRepeatCount(mNativePlayer, mRepeatCount);

            if (marsData.getType() == MarsDataBase.Type.JSON_BIN) {
                MarsDataJsonBin marsDataJsonBin = (MarsDataJsonBin) marsData;
                if (marsDataJsonBin.getBinBytes() != null) {
                    byte[] data = marsDataJsonBin.getBinBytes();
                    mSceneDataPtr = JNIUtil.nativeSceneDataCreate(data, data.length);
                } else {
                    mSceneDataPtr = JNIUtil.nativeSceneDataCreateByPath(((MarsDataJsonBin) marsData).getBinFilePath());
                }
            }
            LogUtil.debug(TAG, "initWithMarsData:mSceneDataPtr " + mSceneDataPtr);
            if (mSceneDataPtr == -1 || mSceneDataPtr == 0) {
                callback.onResult(false, "create scene data fail");
                return;
            }
            for (MarsNativeExtension extension : mExtensions) {
                String[] errMsg = new String[1];
                extension.onSceneDataCreated(mSceneDataPtr, errMsg);
                if (!TextUtils.isEmpty(errMsg[0])) {
                    callback.onResult(false, errMsg[0]);
                    return;
                }
            }

            TaskScheduleUtil.postToUiThreadDelayed(new Runnable() {
                @Override
                public void run() {
                    ViewGroup.LayoutParams layoutParams = new ViewGroup.LayoutParams(
                            ViewGroup.LayoutParams.MATCH_PARENT,
                            ViewGroup.LayoutParams.MATCH_PARENT);
                    mMarsView = new MarsTextureView(getContext(), MarsNativePlayerImpl.this);
                    addView(mMarsView, layoutParams);
                }
            }, 0);
        }
    }

    void showPlaceHolder() {
        if (mPlaceHolder != null) {
            mPlaceHolder.setVisibility(VISIBLE);
        }
        mPlaceHolderVisibility = VISIBLE;
    }

    void hidePlaceHolder() {
        if (mDowngrade) {
            return;
        }
        if (mPlaceHolder != null) {
            mPlaceHolder.setVisibility(GONE);
        }
        mPlaceHolderVisibility = GONE;
    }

    void setPlaceHolder(Bitmap bitmap) {
        LogUtil.debug(TAG, "setPlaceHolder " + bitmap);
        if (mPlaceHolder == null) {
            mPlaceHolder = new ImageView(getContext());
            mPlaceHolder.setScaleType(ImageView.ScaleType.FIT_XY);
            addView(mPlaceHolder, new ViewGroup.LayoutParams(
                    ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
            mPlaceHolder.setVisibility(mPlaceHolderVisibility);
        }
        mPlaceHolder.setImageBitmap(bitmap);
    }

    void setDowngrade() {
        LogUtil.debug(TAG, "setDowngrade");
        mDowngrade = true;
        showPlaceHolder();
        destroy();
    }

    public String getSourceId() {
        return mUrl;
    }

    public boolean isDowngrade() {
        return mDowngrade;
    }

    @Override
    protected void finalize() throws Throwable {
        super.finalize();
        LogUtil.debug(TAG, "finalize");
        destroy();
    }

    private void tryLoadImages(MarsDataBase marsData) {
        List<MarsDataBase.ImageInfo> images = marsData.getImages();
        if (images == null || images.isEmpty()) {
            LogUtil.debug(TAG, "tryLoadImages skip");
            TaskScheduleUtil.postToNormalThread(new Runnable() {
                @Override
                public void run() {
                    setupImages(new ImageProcessor(null));
                }
            });
            return;
        }
        final ArrayList<MarsNativeResourceLoader.MarsNativeImageResource> resList = new ArrayList<>();
        // 是否出现过templateIdx，如果出现过，后面应全是数据模版；如果后面出现非数据模版，应忽略
        boolean hasTemplateIdx = false;
        mResList = resList;
        for (int i = 0; i < images.size(); i++) {
            MarsDataBase.ImageInfo info = images.get(i);
            String url = info.url;
            if (!TextUtils.isEmpty(info.astc)) {
                url = info.astc;
            }
            MarsNativeResourceLoader.MarsNativeImageResource res = new MarsNativeResourceLoader.MarsNativeImageResource(url);
            res.isVideo = info.isVideo;
            res.isTransparentVideo = info.isTransparentVideo;
            if (res.isVideo) {
                res.videoHardDecoder = ConfigUtil.enableVideoHardDecoder(mUrl);
            }
            if (info.templateIdx >= 0) {
                hasTemplateIdx = true;
                res = resList.get(info.templateIdx);
                if (res == null) {
                    continue;
                }
                res.key = info.url; // 如果设置了数据模版，替换掉之前的res的key
            }
            if (!hasTemplateIdx) {
                resList.add(res);
            }
        }
        LogUtil.debug(TAG, "tryLoadImages count:" + resList.size());
        MarsNativeResourceLoader.loadImages(resList, marsData.getDirPath(), mVariables, mVariableBitmaps, mNativePlayer, mSource, new MarsNativeResourceLoader.LoadImageListCallback() {
            @Override
            public void onResult(String errMsg) {
                if (!TextUtils.isEmpty(errMsg)) {
                    LogUtil.error(TAG, "tryLoadImages..e:" + errMsg);
                    TaskScheduleUtil.postToNormalThread(new Runnable() {
                        @Override
                        public void run() {
                            setupImages(null);
                        }
                    });
                    return;
                }
                LogUtil.debug(TAG, "tryLoadImages success");
                TaskScheduleUtil.postToNormalThread(new Runnable() {
                    @Override
                    public void run() {
                        ImageProcessor processor = new ImageProcessor(resList);
                        if (marsData.getFonts() == null || marsData.getFonts().isEmpty()) {
                            // 没有字体，跳过
                            setupImages(processor);
                        } else {
                            tryLoadFonts(marsData, marsData.getDirPath(), processor);
                        }
                    }
                });
            }
        });
    }

    private void tryLoadFonts(MarsDataBase marsData, String dirPath, ImageProcessor imageProcessor) {
        ArrayList<String> urlList = new ArrayList<>();
        ArrayList<MarsDataBase.FontInfo> fonts = marsData.getFonts();
        for (int i = 0; i < fonts.size(); i++) {
            MarsDataBase.FontInfo font = fonts.get(i);
            if (!TextUtils.isEmpty(font.url)) {
                urlList.add(font.url);
            }
        }
        imageProcessor.setTexts(marsData.getTexts(), marsData.getFonts());
        if (urlList.isEmpty()) {
            // 没有线上字体文件，跳过
            setupImages(imageProcessor);
            return;
        }
        MarsNativeResourceLoader.loadFonts(urlList, mSource, dirPath, new MarsNativeResourceLoader.LoadFontListCallback() {
            @Override
            public void onResult(HashMap<String, String> fontPathMap) {
                if (fontPathMap == null) {
                    LogUtil.error(TAG, "tryLoadFonts..e:fontPathMap is null");
                    TaskScheduleUtil.postToNormalThread(new Runnable() {
                        @Override
                        public void run() {
                            setupImages(null);
                        }
                    });
                    return;
                }

                LogUtil.debug(TAG, "tryLoadFonts success");
                TaskScheduleUtil.postToNormalThread(new Runnable() {
                    @Override
                    public void run() {
                        imageProcessor.setFontDataMap(fontPathMap);
                        setupImages(imageProcessor);
                    }
                });
            }
        });
    }

    private void setupImages(ImageProcessor imageProcessor) {
        synchronized (mPtrMutex) {
            final ArrayList<MarsNativeResourceLoader.MarsNativeImageResource> temp = mResList;
            mResList = null;
            if (mSceneDataPtr == 0 || mSceneDataPtr == -1) {
                LogUtil.debug(TAG, "setupImages:ptr is null");
                return;
            }
            if (imageProcessor == null) {
                LogUtil.error(TAG, "setupImages processor is null");
                if (mInitCallback != null) {
                    mInitCallback.onResult(false, "download images fail");
                }
                return;
            }
            Map<String, ImageProcessor.DataInfo> imageDataMap = imageProcessor.process(mVariables);
            if (imageDataMap == null) {
                LogUtil.debug(TAG, "setupImages:dataMap is null");
                if (mInitCallback != null) {
                    mInitCallback.onResult(false, "load images fail");
                }
                return;
            }
            for (Map.Entry<String, ImageProcessor.DataInfo> entry : imageDataMap.entrySet()) {
                String url = entry.getKey();
                ImageProcessor.DataInfo info = entry.getValue();
                if (info.isVideo) {
                    // 视频纹理
                    JNIUtil.nativeSetSceneImageResourceVideo(mSceneDataPtr, url, info.videoRes.videoContext);
                    info.videoRes.videoContext = 0;
                } else if (info.isKtx) {
                    JNIUtil.nativeSetSceneImageResource(mSceneDataPtr, url, info.bytes, info.bytes.length);
                } else {
                    JNIUtil.nativeSetSceneImageResourceBmp(mSceneDataPtr, url, info.bitmap);
                }
            }
            imageProcessor.releaseData();
            JNIUtil.nativeSetSceneData(mNativePlayer, mSceneDataPtr);
            mSceneDataPtr = 0;
            if (mInitCallback != null) {
                mInitCallback.onResult(true, null);
                mInitCallback = null;
            }
        }
    }

    private static synchronized boolean tryLoadSo() {
        if (!sIsLoadSo) {
            try {
                LogUtil.debug(TAG, "load marsnative.so async");
                System.loadLibrary("marsnative");
                sIsLoadSo = true;
            } catch (Throwable e) {
                LogUtil.debug(TAG, "load library failed" + e.toString());
            }
        }
        return sIsLoadSo;
    }

    static String getFormattedSourceId(String url) {
        try {
            if (url.length() > 32 && (url.startsWith("http://") || url.startsWith("https://"))) {
                int idx = url.lastIndexOf("A*");
                if (idx != -1) {
                    return url.substring(idx, Math.min(idx + 32, url.length()));
                }
            }
        } catch (Exception e) {
            LogUtil.error(TAG, "getFormattedSourceId..e:" + e.getMessage());
        }
        return url;
    }

    @Override
    public void onEvent(int type, String msg) {
        LogUtil.debug(TAG, "onEvent:" + type + "," + msg + ".");
        switch (type) {
            case Constants.PLATFORM_EVENT_STATISTICS:
                TaskScheduleUtil.postToNormalThread(new Runnable() {
                    @Override
                    public void run() {
                        if (TextUtils.isEmpty(msg)) {
                            LogUtil.error(TAG, "empty msg for event1");
                            return;
                        }
                        String[] arr = msg.split("_");
                        if (arr.length < 2) {
                            LogUtil.error(TAG, "invalid msg for event1 " + msg);
                            return;
                        }
                        boolean supportCompressedTexture = (arr[0].equals("true"));
                        int glesVersion = Integer.parseInt(arr[1]);
                        MonitorUtil.monitorStatisticsEvent(mUrl, supportCompressedTexture, glesVersion);
                    }
                });
                break;

            case Constants.PLATFORM_EVENT_START:
                TaskScheduleUtil.postToUiThreadDelayed(new Runnable() {
                    @Override
                    public void run() {
                        hidePlaceHolder();
                        if (mFirstScreenCallback != null) {
                            mFirstScreenCallback.run();
                            mFirstScreenCallback = null;
                        }
                    }
                }, 0);
                break;

            case Constants.PLATFORM_EVENT_THREAD_START:
                Thread.currentThread().setName(msg);
                DowngradeUtil.writeResourceIdBegin(mUrl, msg);
                break;

            case Constants.PLATFORM_EVENT_THREAD_END:
                DowngradeUtil.writeResourceIdFinish(mUrl, msg);
                break;

            case Constants.PLATFORM_EVENT_ANIMATION_END:
                TaskScheduleUtil.postToUiThreadDelayed(new Runnable() {
                    @Override
                    public void run() {
                        PlayCallback callback = (mPlayCallbackHolder != null) ? mPlayCallbackHolder.mPlayCallback : null;
                        if (callback != null && TextUtils.equals(mPlayCallbackHolder.mToken, msg)) {
                            callback.onPlayFinish();
                        }
                    }
                }, 0);
                break;

            case Constants.PLATFORM_EVENT_INTERACT_MESSAGE_BEGIN:
            case Constants.PLATFORM_EVENT_INTERACT_MESSAGE_END:
                TaskScheduleUtil.postToUiThreadDelayed(new Runnable() {
                    @Override
                    public void run() {
                        if (mEventListener != null) {
                            mEventListener.onMessageItem(msg,
                                    (type == Constants.PLATFORM_EVENT_INTERACT_MESSAGE_BEGIN)
                                            ? "MESSAGE_ITEM_PHRASE_BEGIN"
                                            : "MESSAGE_ITEM_PHRASE_END");
                        }
                    }
                }, 0);
                break;

            case Constants.PLATFORM_EVENT_EGL_INIT_ERROR:
            case Constants.PLATFORM_EVENT_RUNTIME_ERROR:
                TaskScheduleUtil.postToUiThreadDelayed(new Runnable() {
                    @Override
                    public void run() {
                        // 回调运行时异常
                        PlayCallback callback = (mPlayCallbackHolder != null) ? mPlayCallbackHolder.mPlayCallback : null;
                        if (callback != null) {
                            try {
                                callback.onRuntimeError(msg);
                            } catch (Exception e) {
                                LogUtil.error(TAG, "onRuntimeError..e:" + e.getMessage());
                            }
                        }
                        setDowngrade();
                    }
                }, 0);
                TaskScheduleUtil.postToNormalThread(new Runnable() {
                    @Override
                    public void run() {
                        // 上报埋点
                        MonitorUtil.monitorErrorEvent(mUrl, "runtime_error", msg);
                    }
                });
                break;

            default:
                LogUtil.debug(TAG, "unhandled event:" + type);
                break;
        }
    }

    @Override
    public void onSurfaceCreated(Surface surface) {
        JNIUtil.nativeSetupSurface(mNativePlayer, surface);
    }

    @Override
    public void onSurfaceResize(int width, int height) {
        JNIUtil.nativeResizeSurface(mNativePlayer, width, height);
    }

    @Override
    public void onSurfaceDestroyed() {
        JNIUtil.nativeDestroySurface(mNativePlayer);
    }

    private static final AtomicInteger sPlayerIndex = new AtomicInteger(0);
    public static synchronized int generatePlayerIndex() {
        return sPlayerIndex.incrementAndGet();
    }

    interface InitCallback {
        void onResult(boolean success, String errMsg);
    }

    private static final AtomicInteger sCompleteIdx = new AtomicInteger(0);

    private static class PlayCallbackHolder {
        public PlayCallback mPlayCallback;
        public String mToken;

        PlayCallbackHolder(PlayCallback callback) {
            synchronized (sCompleteIdx) {
                mToken = "p" + sCompleteIdx.incrementAndGet();
            }
            mPlayCallback = callback;
        }
    }

}
