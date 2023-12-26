package com.alipay.mobile.mars.adapter;

import android.support.annotation.Nullable;

import com.alipay.mobile.mars.MarsNativeErrorType;

public class MonitorUtil {
    
    private static final String TAG = "MonitorUtil";

    public static void monitorStartEvent(String sourceId, boolean useIPC, String source, boolean marsPlayer) {
        LogUtil.debug(TAG, String.format(
                "monitorStartEvent, sourceId=%s, useIPC=%s, source=%s, marsPlayer=%s", sourceId, useIPC, source, marsPlayer));
    }

    public static void monitorErrorEvent(String sourceId, MarsNativeErrorType error) {
        monitorErrorEvent(sourceId, error, error.getMsg());
    }

    public static void monitorErrorEvent(String sourceId, String errorType, String errorMsg) {
        LogUtil.debug(TAG, String.format(
                "monitorErrorEvent, sourceId=%s, errorType=%s, errorMsg=%s", sourceId, errorType, errorMsg));
    }


    public static void monitorErrorEvent(String sourceId, MarsNativeErrorType error, String msg) {
        LogUtil.debug(TAG, String.format(
                "monitorErrorEvent, sourceId=%s, errorType=%s, errorMsg=%s", sourceId, error.getType(), error.getMsg()));
    }

    public static void monitorStatisticsEvent(String sourceId, boolean supportCompressedTexture, int glesVersion) {
        LogUtil.debug(TAG, String.format(
                "monitorStatisticsEvent, sourceId=%s, supportCompressedTexture=%s, glesVersion=%s", sourceId, supportCompressedTexture, glesVersion));
    }

    public static void monitorCleanEvent(String type, String operation, boolean success) {
        monitorCleanEvent(type, operation, success, null, null);
    }

    /**
     * 资源清理上报埋点
     * @param type 操作类型，清理、记录等
     * @param operation 具体操作
     * @param success 是否成功
     * @param itemKey 有问题的文件名
     * @param reason 问题原因
     */
    public static void monitorCleanEvent(String type, String operation, boolean success, @Nullable String itemKey,
            @Nullable String reason) {
        LogUtil.debug(TAG, String.format(
                "monitorCleanEvent, type=%s, operation=%s, success=%s, itemKey=%s, reason=%s", type, operation, success,
                itemKey, reason));
    }
    
}
