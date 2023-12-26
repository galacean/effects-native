package com.alipay.mobile.mars.player;

import android.content.Context;
import android.graphics.Bitmap;
import android.support.annotation.Nullable;
import android.text.TextUtils;

import com.alipay.mobile.mars.util.IOUtil;

import com.alipay.mobile.mars.adapter.TaskScheduleUtil;
import com.alipay.mobile.mars.util.CommonUtil;
import com.alipay.mobile.mars.util.Constants;
import com.alipay.mobile.mars.adapter.LogUtil;

import java.io.FileInputStream;
import java.io.InputStream;

class PlaceHolderUtil {

    private final static String TAG = "PlaceHolderUtil";

    static Bitmap loadPlaceHolderFromLocal(Context context, String url, Bitmap bitmap, String[] errMsg) {
        if (bitmap != null) {
            // 如果有传入图片，直接使用图片
            return bitmap;
        }
        if (TextUtils.isEmpty(url)) {
            LogUtil.debug(TAG, "loadPlaceHolder no resource");
            return null;
        }
        InputStream is = null;
        Bitmap bmp = null;
        try {
            // 优先尝试从包内加载
            String path = MarsNativeResourceLoader.loadFilePathFromLocal(url);
            if (path != null) {
                is = context.getAssets().open(url.substring(Constants.RES_PATH_ASSETS_PREFIX.length()));
            } else {
                // 包内找不到再看多媒体缓存有没有
                path = MarsNativeResourceLoader.loadFilePathFromCache(url);
                if (path != null) {
                    is = new FileInputStream(path);
                }
            }
        } catch (Exception e) {
            errMsg[0] = "Exception:" + e.getMessage();
            return null;
        } finally {
            if (is != null) {
                byte[] bytes = CommonUtil.readFileStreamBinaryAndClose(is);
                if (bytes == null || bytes.length == 0) {
                    errMsg[0] = "bytes is null";
                } else {
                    bmp = CommonUtil.decodeImage(bytes);
                    if (bmp == null) {
                        errMsg[0] = "decodeImage fail," + url;
                    }
                }

                IOUtil.closeStream(is);
            }
        }
        return bmp;
    }

    static void loadPlaceHolder(Context context, final String url, Bitmap bitmap, String source, final MarsNativeResourceLoader.LoadBitmapCallback callback) {
        // 尝试从本地加载
        String[] errMsg = new String[1];
        errMsg[0] = null;
        Bitmap localBmp = loadPlaceHolderFromLocal(context, url, bitmap, errMsg);
        if (localBmp != null) {
            LogUtil.debug(TAG, "loadPlaceHolderFromLocal," + url);
            callbackOnUIThread(callback, localBmp, null);
            return;
        } else if (errMsg[0] != null) {
            callbackOnUIThread(callback, null, errMsg[0]);
            return;
        }

        if (TextUtils.isEmpty(url)) {
            callbackOnUIThread(callback, null, null);
            return;
        }

        // 不是本地路径，加载url
        MarsNativeResourceLoader.loadBitmap(url, source, new MarsNativeResourceLoader.LoadBitmapCallback() {
            @Override
            public void onResult(Bitmap bitmap, String errMsg) {
                LogUtil.debug(TAG, "loadBitmap," + url);
                callbackOnUIThread(callback, bitmap, errMsg);
            }
        });
    }

    private static void callbackOnUIThread(
            final MarsNativeResourceLoader.LoadBitmapCallback callback,
            @Nullable final Bitmap bitmap, @Nullable final String errMsg) {
        TaskScheduleUtil.postToUiThreadDelayed(new Runnable() {
            @Override
            public void run() {
                callback.onResult(bitmap, errMsg);
            }
        }, 0);
    }

}
