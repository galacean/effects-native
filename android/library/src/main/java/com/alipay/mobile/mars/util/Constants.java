package com.alipay.mobile.mars.util;

public class Constants {

    public static final int DEVICE_LEVEL_HIGH = 4;

    public static final int DEVICE_LEVEL_MEDIUM = 2;

    public static final int DEVICE_LEVEL_LOW = 1;

    public static final String STR_DEVICE_LEVEL_HIGH = "high";

    public static final String STR_DEVICE_LEVEL_MEDIUM = "medium";

    public static final String STR_DEVICE_LEVEL_LOW = "low";

    public static final int ERROR_LOAD_BITMAP_FAIL = 1001;

    public static final int ERROR_LOAD_ZIP_FAIL = 1002;

    public static final int ERROR_CREATE_MARS_DATA_FAIL = 1003;

    public static final int ERROR_CREATE_PLAYER_FAIL = 1004;

    public static final int ERROR_DOWNGRADE = 1005;

    public static final String RES_PATH_ASSETS_PREFIX = "assets://";

    // 支付宝不能支持访问本地磁盘，打开要拉安全同学评估 from 夙兮
//    public static final String RES_PATH_FILE_PREFIX = "file://";

    public static final int NATIVE_EVENT_PAUSE = 1;

    public static final int NATIVE_EVENT_RESUME = 2;

    public static final int PLATFORM_EVENT_STATISTICS = 1;
    public static final int PLATFORM_EVENT_START = 2;
    public static final int PLATFORM_EVENT_THREAD_START = 3;
    public static final int PLATFORM_EVENT_THREAD_END = 4;
    public static final int PLATFORM_EVENT_ANIMATION_END = 5;
    public static final int PLATFORM_EVENT_INTERACT_MESSAGE_BEGIN = 6;
    public static final int PLATFORM_EVENT_INTERACT_MESSAGE_END = 7;
    public static final int PLATFORM_EVENT_EGL_INIT_ERROR = 100;
    public static final int PLATFORM_EVENT_RUNTIME_ERROR = 101;

}
