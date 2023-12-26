package com.alipay.mobile.mars.player;

import android.content.Context;
import android.graphics.Bitmap;
import android.text.TextUtils;

import com.alipay.mobile.mars.adapter.ContextUtil;
import com.alipay.mobile.mars.adapter.MD5Util;
import com.alipay.mobile.mars.adapter.DownloadUtil;
import com.alipay.mobile.mars.adapter.TaskScheduleUtil;
import com.alipay.mobile.mars.util.CommonCallbacks;
import com.alipay.mobile.mars.util.CommonUtil;
import com.alipay.mobile.mars.util.Constants;
import com.alipay.mobile.mars.util.JNIUtil;
import com.alipay.mobile.mars.util.JNIUtil.CommonJNICallback;
import com.alipay.mobile.mars.adapter.LogUtil;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.lang.ref.WeakReference;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

public class MarsNativeResourceLoader {

    private final static String TAG = "MarsNativeResourceLoader";

    // 获取本地文件路径，目前只支持包内路径
    static String loadFilePathFromLocal(final String url) {
        if (url.startsWith(Constants.RES_PATH_ASSETS_PREFIX)) {
            // 包内路径
            return url;
        }
        return null;
    }

    // 获取下载缓存的资源路径
    static String loadFilePathFromCache(final String url) {
        return DownloadUtil.getDownloadCachePath(url);
    }

    static void loadZip(final String url, String bizType, long validTimeStamp, String md5, final LoadZipCallback callback) {
        // 拼接最终保存路径
        String urlMd5 = MD5Util.getMD5String(url);
        String dstDirPath = FileOperation.getMarsDir() + urlMd5;

        long timeStamp = System.currentTimeMillis();
        if (validTimeStamp > timeStamp) {
            // 如果传入的时间戳晚于当前时间，使用传入的
            timeStamp = validTimeStamp;
        }
        if (FileOperation.checkDirExist(dstDirPath, true, timeStamp)) {
            callback.onSuccess(dstDirPath);
            return;
        }
        FileOperation.downloadAndUnzip(url, bizType, dstDirPath, timeStamp, md5, new FileOperation.DownloadZipCallback() {
            @Override
            public void onResult(String dstPath, String errMsg) {
                if (TextUtils.isEmpty(errMsg)) {
                    callback.onSuccess(dstPath);
                } else {
                    callback.onError(errMsg);
                }
            }
        });
    }

    static void loadBitmap(String url, String bizType, final LoadBitmapCallback callback) {
        DownloadUtil.downloadFile(url, bizType, null, new DownloadUtil.DownloadFileCallback() {
            @Override
            public void onSuccess(InputStream is, String filePath) {
                try {
                    byte[] bytes = CommonUtil.readFileStreamBinaryAndClose(is);
                    if (bytes == null || bytes.length == 0) {
                        callback.onResult(null, "bytes is null");
                        return;
                    }
                    Bitmap bitmap = CommonUtil.decodeImage(bytes);
                    if (bitmap != null) {
                        callback.onResult(bitmap, null);
                    } else {
                        callback.onResult(null, "decode image fail");
                    }
                } catch (Exception e) {
                    callback.onResult(null, e.getMessage());
                }
            }

            @Override
            public void onError(String errMsg) {
                callback.onResult(null, errMsg);
            }
        });
    }

    static void loadImages(List<MarsNativeImageResource> resList, String dirPath, Map<String, String> variables, Map<String, Bitmap> variablesBitmap, final int playerIdx, String bizType, final LoadImageListCallback callback) {
        if (resList == null || resList.isEmpty()) {
            TaskScheduleUtil.postToNormalThread(new Runnable() {
                @Override
                public void run() {
                    callback.onResult("");
                }
            });
            return;
        }
        /*
          0: 图片总数
          1: 加载结束个数
          2: 是否成功：1成功，0失败
         */
        final int[] temp = { resList.size(), 0, 1 };
        try {
            for (int i = 0; i < resList.size(); i++) {
                final MarsNativeImageResource res = resList.get(i);

                // 检查&替换动态参数
                if (variables != null) {
                    String tmp = variables.get(res.key);
                    if (!TextUtils.isEmpty(tmp)) {
                        res.realUrl = tmp;
                    }
                }
                if (variablesBitmap != null) {
                    Bitmap bitmap = variablesBitmap.get(res.key);
                    if (bitmap!= null) {
                        res.bitmap = bitmap;
                    }
                }

                loadImageInternal(res, dirPath, playerIdx, bizType, new CommonCallbacks.SuccessErrorCallback() {
                    @Override
                    public void onResult(boolean success, String errMsg) {
                        synchronized (temp) {
                            if (temp[2] == 0) {
                                // 之前已经下载失败，不再处理
                                return;
                            }
                            if (!success) {
                                temp[2] = 0;
                                callback.onResult("loadFile(" + res.realUrl + ") fail," + errMsg);
                                return;
                            }
                            ++temp[1];
                            LogUtil.debug(TAG, "loadImage(" + res.realUrl + ") success");
                            if (temp[0] == temp[1]) {
                                callback.onResult("");
                            }
                        }
                    }
                });
            }
        } catch (Exception e) {
            LogUtil.error(TAG, "loadImages..e:" + e.getMessage());
            temp[2] = 0;
            callback.onResult(e.getMessage());
        }
    }

    static void loadFonts(List<String> urlList, String  bizType, String dirPath, final LoadFontListCallback callback) {
        if (urlList == null || urlList.isEmpty()) {
            TaskScheduleUtil.postToNormalThread(new Runnable() {
                @Override
                public void run() {
                    callback.onResult(null);
                }
            });
            return;
        }

        HashMap<String, String> fontPathMap = new HashMap<>();
        try {
            final int[] temp = { urlList.size(), 0, 1 }; // 0: 字体数量，1: 已加载数量，2: 是否成功
            for (int i = 0; i < urlList.size(); i++) {
                final String url = urlList.get(i);
                loadFontInternal(url, dirPath, bizType, new LoadFontInternalCallback() {
                    @Override
                    public void onResult(String fontPath, String errMsg) {
                        synchronized (temp) {
                            if (temp[2] == 0) {
                                // 之前已经加载失败，不再处理
                                return;
                            }
                            if (fontPath == null) {
                                temp[2] = 0;
                                callback.onResult(null);
                                return;
                            }

                            fontPathMap.put(url, fontPath);
                            ++temp[1];
                            LogUtil.debug(TAG, "loadFont (" + url + ") success");

                            if (temp[0] == temp[1]) {
                                callback.onResult(fontPathMap);
                            }
                        }
                    }
                });
            }
        } catch (Exception e) {
            LogUtil.error(TAG, "loadFonts..e:" + e.getMessage());
            callback.onResult(null);
        } finally {
            LogUtil.debug(TAG, "loadFonts: " + Integer.toString(urlList.size()));
        }
    }

    private static void loadImageInternal(MarsNativeImageResource res, final String dirPath, final int playerIdx, String bizType, final CommonCallbacks.SuccessErrorCallback callback) {
        String url = res.realUrl;
        if (TextUtils.isEmpty(url)) {
            TaskScheduleUtil.postToNormalThread(new Runnable() {
                @Override
                public void run() {
                    callback.onResult(false, "url is null");
                }
            });
            return;
        }

        if (res.bitmap != null) {
            // 已有图片，无需加载
            TaskScheduleUtil.postToNormalThread(new Runnable() {
                @Override
                public void run() {
                    callback.onResult(true, "");
                }
            });
            return;
        }

        // 文本节点底图
        if (url.startsWith("text://")) {
            String[] components = url.split("_");
            if (components.length == 3) {
                try {
                    float width = Float.parseFloat(components[1]);
                    float height = Float.parseFloat(components[2]);
                    res.bitmap = MarsNativeResourceLoader.generateTransparentImageWithWidth(width, height);
                    TaskScheduleUtil.postToNormalThread(new Runnable() {
                        @Override
                        public void run() { callback.onResult(true, ""); }
                    });
                } catch (Exception e) {
                    TaskScheduleUtil.postToNormalThread(new Runnable() {
                        @Override
                        public void run() {
                            callback.onResult(false, "url is text://xx_xx_xx, but failed to get width, height or bitmap.");
                        }
                    });
                }
            } else {
                TaskScheduleUtil.postToNormalThread(new Runnable() {
                    @Override
                    public void run() {
                        callback.onResult(false, "url is wrong, text://xx_xx_xx expected.");
                    }
                });
            }
            return;
        }


        if (!url.startsWith("https://") && !url.startsWith("http://")) {
            TaskScheduleUtil.postToNormalThread(new Runnable() {
                @Override
                public void run() {
                    String filePath = dirPath + File.separator + url;
                    if (res.isVideo) {
                        if (new File(filePath).exists()) {
                            res.videoRes = new MarsNativeVideoResource(
                                    playerIdx, filePath, res.isTransparentVideo,
                                    res.videoHardDecoder, callback);
                        } else {
                            callback.onResult(false, "read video file failed");
                        }
                        return;
                    }
                    InputStream is = null;
                    try {
                        Context context = ContextUtil.getApplicationContext();
                        if (filePath.startsWith(Constants.RES_PATH_ASSETS_PREFIX)) {
                            is = context.getAssets().open(filePath.substring(Constants.RES_PATH_ASSETS_PREFIX.length()));
                        } else {
                            is = new FileInputStream(filePath);
                        }
                        byte[] bytes = CommonUtil.readFileStreamBinaryAndClose(is);
                        res.data = bytes;
                    } catch (Exception e) {
                        LogUtil.error(TAG, "load local image..e:" + e.getMessage());
                        callback.onResult(false, e.getMessage());
                    }
                    callback.onResult(true, "");
                }
            });
            return;
        }

        DownloadUtil.downloadFile(url, bizType, null, new DownloadUtil.DownloadFileCallback() {
            @Override
            public void onSuccess(InputStream is, String filePath) {
                if (res.isVideo) {
                    res.videoRes = new MarsNativeVideoResource(
                            playerIdx, filePath, res.isTransparentVideo, res.videoHardDecoder,
                            callback);
                    return;
                }
                res.data = CommonUtil.readFileStreamBinaryAndClose(is);
                callback.onResult(true, "");
            }

            @Override
            public void onError(String errMsg) {
                callback.onResult(false, errMsg);
            }
        });
    }

    private static void loadFontInternal(String url, final String dirPath, String bizType, final LoadFontInternalCallback callback) {
        if (TextUtils.isEmpty(url)) {
            TaskScheduleUtil.postToNormalThread(new Runnable() {
                @Override
                public void run() {
                    callback.onResult(null, "url is null");
                }
            });
            return;
        }

        if (!url.startsWith("https://") && !url.startsWith("http://")) {
            TaskScheduleUtil.postToNormalThread(new Runnable() {
                @Override
                public void run() {
                    String filePath = dirPath + File.separator + url;
                    if (new File(filePath).exists()) {
                        callback.onResult(filePath, "");
                    } else {
                        callback.onResult(null, "read font file failed");
                    }
                }
            });
            return;
        }

        DownloadUtil.downloadFile(url, bizType, null, new DownloadUtil.DownloadFileCallback() {
            @Override
            public void onSuccess(InputStream is, String filePath) {
                callback.onResult(filePath, "");
            }

            @Override
            public void onError(String errMsg) {
                callback.onResult(null, errMsg);
            }
        });
    }

    public static Bitmap generateTransparentImageWithWidth(float width, float height) {
        return Bitmap.createBitmap((int) width, (int) height, Bitmap.Config.ARGB_8888);
    }

    public static class MarsNativeVideoResource {
        public long videoContext = 0;

        private CommonCallbacks.SuccessErrorCallback mComplete;

        public MarsNativeVideoResource(int playerIdx, String filePath, boolean transparent, boolean hardDecoder, CommonCallbacks.SuccessErrorCallback complete) {
            mComplete = complete;
            WeakReference<MarsNativeVideoResource> weakThiz = new WeakReference<>(this);
            videoContext = JNIUtil.nativeCreateVideoContextAndPrepare(
                    playerIdx, filePath, MD5Util.getMD5String(filePath), transparent, hardDecoder,
                    new JNICallback(weakThiz));
            if (videoContext == 0) {
                LogUtil.error("MarsNativeVideoResource", "create video context fail");
                mComplete.onResult(false, "create video context fail");
                mComplete = null;
            }
        }

        @Override
        protected void finalize() throws Throwable {
            super.finalize();
            LogUtil.debug("MarsNativeVideoResource", "MarsNativeVideoResource finalize");
        }

        private static class JNICallback extends CommonJNICallback {
            private final WeakReference<MarsNativeVideoResource> weakThiz;

            JNICallback(WeakReference<MarsNativeVideoResource> weakReference) {
                weakThiz = weakReference;
            }

            @Override
            public void onCallback(String[] params) {
                MarsNativeVideoResource thiz = weakThiz.get();
                if (thiz == null || thiz.mComplete == null) {
                    return;
                }
                LogUtil.debug("LoadVideoCallback", Arrays.toString(params));;
                boolean success = params[0].equalsIgnoreCase("true");
                if (success) {
                    thiz.mComplete.onResult(true, "");
                } else {
                    thiz.mComplete.onResult(false, (String) params[1]);
                }
                thiz.mComplete = null;
            }
        }
    }

    public static class MarsNativeImageResource {
        public String key;
        public String realUrl;
        public boolean isVideo = false;
        public boolean isTransparentVideo = false;
        public boolean videoHardDecoder = false;
        public MarsNativeVideoResource videoRes = null;
        public byte[] data = null;
        public Bitmap bitmap = null;

        public MarsNativeImageResource(String url) {
            this.key = url;
            this.realUrl = url;
        }
    }

    public interface LoadBitmapCallback {
        void onResult(Bitmap bitmap, String errMsg);
    }

    interface LoadZipCallback {
        void onSuccess(String dirPath);

        void onError(String errMsg);
    }

    interface LoadImageListCallback {
        void onResult(String errMsg);
    }

    interface LoadFontListCallback {
        void onResult(HashMap<String, String> fontPathMap);
    }

    interface LoadFontInternalCallback {
        void onResult(String fontPath, String errMsg);
    }

}
