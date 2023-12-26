package com.alipay.mobile.mars;

/**
 * Error埋点Type需要指定，方便洗数据，ErrorMsg可以自定义
 * TYPE:
 *      - init_error
 *      - runtime_error
 *      - other_error
 */
public enum MarsNativeErrorType {

    EMPTY_BIN_FILE("init_error", "empty bin file"),

    EMPTY_PLAYER("init_error", "empty player"),

    LOAD_SO_FAILED("init_error", "load so failed"),

    CONTEXT_DESTROYED("init_error", "context destroyed"),

    INVALID_IMAGE_DATA("init_error", "invalid image data"),

    INVALID_PARAM("init_error", "invalid params"),

    SURFACE_CREATE_WITHOUT_PLAYERID("init_error", "surface create without playerID"),

    SNAPSHOT_WHITE_SCREEN("other_error", "snapshot_white_scree"),

    EGL_CREATE_FAIL("init_error", "create egl context fail"),

    DECODE_BIN_FAIL("init_error", "decode bin fail"),

    DECODE_IMAGE_FAIL("init_error", "decode image fail");

    private String mErrorType;

    private String mErrorMsg;

    MarsNativeErrorType(String errType, String errMsg) {
        mErrorMsg = errMsg;
        mErrorType = errType;
    }

    public String getType () {
        return mErrorType;
    }

    public String getMsg() {
        return mErrorMsg;
    }
}
