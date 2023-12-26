package com.alipay.mobile.mars.adapter;

import android.content.Context;
import android.content.SharedPreferences;

import com.alipay.mobile.mars.adapter.LogUtil;

public class StorageUtil {

    private final static String TAG = "StorageUtil";

    // 记录上次清理文件时间
    public final static String KEY_LAST_CLEAN_TIMESTAMP = "MarsNativePurgeTime";

    private final static String sGroup = "com.alipay.mobile.mars.player";

    private final static int sMode = Context.MODE_PRIVATE;

    private static SharedPreferences sSP;

    private static synchronized void initIfNot() {
        if (sSP == null) {
            Context appContext = ContextUtil.getApplicationContext();
            sSP = appContext.getSharedPreferences(sGroup, sMode);
        }
    }

    public static void putLong(String key, long value) {
        try {
            initIfNot();
            if (sSP == null) {
                LogUtil.error(TAG, "putLong:sEdit == null");
                return;
            }
            SharedPreferences.Editor editor = sSP.edit();
            editor.putLong(key, value);
            editor.apply();
        } catch (Exception e) {
            LogUtil.error(TAG, "putLong..e:" + e.getMessage());
        }
    }

    public static long getLong(String key, long defaultVal) {
        try {
            initIfNot();
            if (sSP == null) {
                LogUtil.error(TAG, "getLong:sSP == null");
                return defaultVal;
            }
            return sSP.getLong(key, defaultVal);
        } catch (Exception e) {
            LogUtil.error(TAG, "getLong..e:" + e.getMessage());
            return defaultVal;
        }
    }

}
