package com.alipay.mobile.mars.adapter;

import android.content.Context;

public class ContextUtil {
    
    private static Context applicationContext;

    public static void setApplicationContext(Context context) {
        applicationContext = context;
    }

    public static Context getApplicationContext() {
        return applicationContext;
    }
    
}
