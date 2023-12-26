package com.alipay.mobile.mars.adapter;

import android.util.Log;

public class LogUtil {

    private static final String GLOBAL_TAG = "MN::";

    private static ILogger instance = new ILogger() {
        @Override
        public void debug(String tag, String msg) {
            Log.d(tag, msg);
        }

        @Override
        public void error(String tag, String msg) {
            Log.e(tag, msg);
        }
    };

    public static void setLogger(ILogger logger) {
        instance = logger;
    }

    public static ILogger getLogger() {
        return instance;
    }

    public static void debug(String tag, String msg) {
        if (instance != null) {
            instance.debug(GLOBAL_TAG, tag + ":" + msg);
        }
    }

    public static void error(String tag, String msg) {
        if (instance != null) {
            instance.error(GLOBAL_TAG, tag + ":" + msg);
        }
    }

    public interface ILogger {
        void debug(String tag, String msg);

        void error(String tag, String msg);
    }

}
