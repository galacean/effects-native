package com.alipay.mobile.mars.adapter;

public class DowngradeUtil {

    private static final String BIZ_ID = "MarsNative";

    public static DowngradeResult getDowngradeResult(String resId) {
        DowngradeResult result = new DowngradeResult();
        result.mDowngrade = false;
        result.mReason = null;
        return result;
    }

    public static void writeResourceIdBegin(String resId, String threadName) {
        // left blank
    }

    public static void writeResourceIdFinish(String resId, String threadName) {
        // left blank
    }

    public static class DowngradeResult {

        private boolean mDowngrade = false;

        private String mReason;

        private int mDeviceLevel;

        public boolean getDowngrade() {
            return mDowngrade;
        }

        public String getReason() {
            return mReason;
        }

        public int getDeviceLevel() {
            return mDeviceLevel;
        }
        
    }

}
