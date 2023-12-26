package com.alipay.mobile.mars.util;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Looper;
import android.text.TextUtils;

import com.alipay.mobile.mars.adapter.LogUtil;
import java.io.ByteArrayOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.text.SimpleDateFormat;
import java.util.Date;
import java.util.Locale;

public class CommonUtil {
    public static float clamp(float number, float min, float max) {
        return Math.max(min, Math.min(max, number));
    }

    public static float lerp(float a, float b, float percentage) {
        return a + percentage * (b - a);
    }

    // https://www.khronos.org/registry/KTX/specs/1.0/ktxspec_v1.html#:~:text=KTX%E2%84%A2%20is%20a%20format,object%20from%20the%20file%20contents.
    private static final byte[] sKtxIdentifier = {(byte) 0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, (byte) 0xBB, 0x0D, 0x0A, 0x1A, 0x0A};
    public static boolean isKtxImageData(byte[] data) {
        try {
            if (data == null) {
                return false;
            }
            if (data.length < sKtxIdentifier.length) {
                return false;
            }
            for (int i = 0; i < sKtxIdentifier.length; i++) {
                if (data[i] != sKtxIdentifier[i]) {
                    return false;
                }
            }
            return true;
        } catch (Exception e) {
            LogUtil.error("CommonUtil", "isKtxImageData error: " + e.getMessage());
        }
        return false;
    }

    public static Bitmap decodeImage(byte[] data) {
        try {
            return BitmapFactory.decodeByteArray(data, 0, data.length);
        } catch (Throwable e) {
            LogUtil.error("CommonUtil", "decodeImage error: " + e.getMessage());
        }
        return null;
    }

    public static byte[] readFileStreamBinaryAndClose(final InputStream is) {
        if (is == null) return null;
        ByteArrayOutputStream os = null;
        try {
            os = new ByteArrayOutputStream();
            byte[] b = new byte[8192];
            int len;
            while ((len = is.read(b, 0, 8192)) != -1) {
                os.write(b, 0, len);
            }
            return os.toByteArray();
        } catch (IOException e) {
            LogUtil.error("CommonUtil", "readFileStreamBinary..io:" + e.getMessage());
            return null;
        } catch (OutOfMemoryError oom) {
            LogUtil.error("CommonUtil", "readFileStreamBinary..oom:" + oom.getMessage());
            return null;
        } finally {
            try {
                is.close();
            } catch (IOException e) {
                LogUtil.error("CommonUtil", "readFileStreamBinary..close i:" + e.getMessage());
            }
            try {
                if (os != null) {
                    os.close();
                }
            } catch (IOException e) {
                LogUtil.error("CommonUtil", "readFileStreamBinary..close o:" + e.getMessage());
            }
        }
    }

    /**
     * 读取assets目录下的文件列表
     * @param context context
     * @param assetsDirPath 目录，不包含assets://
     * @return 文件列表，null表示读取失败
     */
    public static String[] readAssetsDir(Context context, String assetsDirPath) {
        try {
            return context.getAssets().list(assetsDirPath);
        } catch (Exception e) {
            LogUtil.error("CommonUtil", "readAssetsDir..e:" + e.getMessage());
        }
        return null;
    }

    /**
     * 读取assets目录下的二进制文件
     * @param context context
     * @param assetsPath 目录，不包含assets://
     * @return 二进制数据，null表示读取失败
     */
    public static byte[] readAssetsBinary(Context context, String assetsPath) {
        try {
            InputStream is = context.getAssets().open(assetsPath);
            return readFileStreamBinaryAndClose(is);
        } catch (Exception e) {
            LogUtil.error("CommonUtil", "readAssetsStreamBinary..e:" + e.getMessage());
        }
        return null;
    }

    public static int parseDeviceLevelString(String levelStr) {
        if (TextUtils.isEmpty(levelStr)) {
            return Constants.DEVICE_LEVEL_HIGH;
        }
        if (levelStr.equalsIgnoreCase(Constants.STR_DEVICE_LEVEL_HIGH)) {
            return Constants.DEVICE_LEVEL_HIGH;
        }
        if (levelStr.equalsIgnoreCase(Constants.STR_DEVICE_LEVEL_MEDIUM)) {
            return Constants.DEVICE_LEVEL_MEDIUM;
        }
        if (levelStr.equalsIgnoreCase(Constants.STR_DEVICE_LEVEL_LOW)) {
            return Constants.DEVICE_LEVEL_LOW;
        }
        return Constants.DEVICE_LEVEL_HIGH;
    }

    public static boolean isMainThread() {
        return Looper.getMainLooper().getThread() == Thread.currentThread();
    }

    public static String formatTimeStamp(long timeStamp) {
        try {
            SimpleDateFormat sdf = new SimpleDateFormat("yyyy-MM-dd HH:mm:ss", Locale.CHINA);
            return sdf.format(new Date(timeStamp));
        } catch (Exception e) {
            LogUtil.error("CommonUtil", "formatTimeStamp.e:" + e.getMessage());
            return String.valueOf(timeStamp);
        }
    }

}
