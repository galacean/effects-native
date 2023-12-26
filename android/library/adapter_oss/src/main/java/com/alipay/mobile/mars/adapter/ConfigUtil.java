package com.alipay.mobile.mars.adapter;

import android.text.TextUtils;

import com.alibaba.fastjson.JSONObject;

import java.math.BigInteger;
import java.security.MessageDigest;

public class ConfigUtil {

    private static final String FORCE_DOWNGRADE_COMPRESSEDTEXTURE = "mn_forceDowngradeCompressed";

    private static final String FORCE_DOWNGRADE_MN = "mn_forceDowngradeMN";

    private static final String FORCE_DOWNGRADE_RES_ID = "mn_dg_";

    private static final String ENABLE_IPC_SERVICE = "mn_enableIPCService";

    private static final String ENABLE_SURFACE_SCALE = "mn_enableSurfaceScale";

    private static final String ENABLE_IPC_SERVICE_PRELOAD = "mn_enableIPCServicePreload";

    private static final String GET_RENDER_LEVEL = "mn_getRenderLevel";

    private static final String DISABLE_NATIVE_DEFAULT_USE_KTX = "mn_defaultUseKtx";

    public static boolean forceDowngradeCompressTexture() {
        String value = getConfig(FORCE_DOWNGRADE_COMPRESSEDTEXTURE);
        if (TextUtils.isEmpty(value)) {
            return false;
        }

        return TextUtils.equals(value, "1");
    }

    public static boolean defaultUseKtx() {
        String value = getConfig(DISABLE_NATIVE_DEFAULT_USE_KTX);
        if (TextUtils.isEmpty(value)) {
            return true;
        }

        return TextUtils.equals(value, "1");
    }

    public static boolean forceDowngradeMN() {
        String value = getConfig(FORCE_DOWNGRADE_MN);
        if (TextUtils.isEmpty(value)) {
            return false;
        }

        return TextUtils.equals(value, "1");
    }

    public static boolean forceDowngradeByResId(String resId) {
        try {
            resId = genMD5(resId);
            return forceDowngradeByResScene(resId);
        } catch (Exception e) {
            LogUtil.error("Config", e.getMessage());
            return false;
        }
    }

    public static boolean forceDowngradeByResScene(String scene) {
        try {
            if (TextUtils.isEmpty(scene)) {
                return false;
            }

            String value = getConfig(FORCE_DOWNGRADE_RES_ID + scene);
            if (TextUtils.isEmpty(value)) {
                return false;
            }

            return TextUtils.equals(value, "1");
        } catch (Exception e) {
            LogUtil.error("Config", e.getMessage());
            return false;
        }
    }

    public static boolean enableIPCService(String sourceId) {
        return false;
    }

    public static boolean enableSurfaceScale(String sourceId) {
        if (TextUtils.isEmpty(sourceId)) {
            return false;
        }

        String value = getConfig(ENABLE_SURFACE_SCALE);
        if (TextUtils.isEmpty(value)) {
            return false;
        }

        if (value.contains(sourceId) || value.contains("all")) {
            return true;
        }

        return false;
    }


    public static boolean enableIPCServicePreLoad() {
        String value = getConfig(ENABLE_IPC_SERVICE_PRELOAD);
        if (TextUtils.isEmpty(value)) {
            return false;
        }

        return TextUtils.equals(value, "1");
    }

    // 1: low, 2: medium, 4: high
    public static int getRenderLevel(int defaultVal) {
        String value = getConfig(GET_RENDER_LEVEL);
        if (TextUtils.isEmpty(value)) {
            return defaultVal;
        }

        try {
            return Integer.parseInt(value);
        } catch (Exception e) {
            LogUtil.error("ConfigCenter", "getRenderLevel..e:" + e.getMessage());
            return defaultVal;
        }
    }

    // 视频是否硬解码
    public static boolean enableVideoHardDecoder(String resId) {
        try {
            String content = getConfig("mn_enableVideoHardDecoder");
            if (TextUtils.isEmpty(content)) { // 开关为空，不用
                return false;
            }
            JSONObject dictionary = JSONObject.parseObject(content);
            if (dictionary == null) { // 开关转json为空，不用
                LogUtil.error("ConfigCenter", "enableVideoHardDecoder empty dict");
                return false;
            }
            String whiteList = dictionary.getString("whiteList");
            if (TextUtils.equals("all", whiteList)) { // 白名单为all，判断是否在黑名单
                String blackList = dictionary.getString("blackList");
                if (TextUtils.isEmpty(blackList)) { // 不在黑名单，用
                    return true;
                }
                if (blackList.contains(resId)) {// 在黑名单，不用
                    return false;
                }
                // 不在黑名单，用
                LogUtil.debug("ConfigCenter", "enableVideoHardDecoder true");
                return true;
            } else { // 白名单不为all，判断是否包含当前resId
                if (TextUtils.isEmpty(whiteList)) {
                    return false;
                }
                return whiteList.contains(resId);
            }
        } catch (Throwable e) {
            LogUtil.error("ConfigCenter", "enableVideoHardDecoder..e:" + e.getMessage());
            return false;
        }
    }

    public static String getStringValue(final String key) {
        return getConfig(key);
    }

    public static String getConfig(String key) {
        return "";
    }
    
    private static String genMD5(String plainText) {
        String md5Code = null;
        if (plainText == null || plainText.isEmpty()) {
            return null;
        }
        try {
            byte[] md5Bytes = MessageDigest.getInstance("md5").digest(
                    plainText.getBytes());
            md5Code = new BigInteger(1, md5Bytes).toString(16).toLowerCase();
            int md5Length = md5Code.length();
            for (int i = 0; i < 32 - md5Length; i++) {
                md5Code = "0" + md5Code;
            }
            if (md5Code.length() > 32) {
                md5Code = md5Code.substring(0, 32);
            }
        } catch (Throwable tr) {
            LogUtil.error("Config", tr.toString());
        }
        return md5Code;
    }
    
}
