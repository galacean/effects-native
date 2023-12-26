package com.alipay.mobile.mars;

public interface MarsNativeListener {

    void onInitAnimation(boolean success, String err);

    void onRenderException(String err);

    void onDispatchEvent(String name, String extra);

}
