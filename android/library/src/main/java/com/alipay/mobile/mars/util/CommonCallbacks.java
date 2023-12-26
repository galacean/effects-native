package com.alipay.mobile.mars.util;

public class CommonCallbacks {

    public interface SuccessErrorCallback {
        void onResult(boolean success, String errMsg);
    }

}
