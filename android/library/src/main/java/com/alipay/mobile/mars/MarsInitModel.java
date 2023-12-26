package com.alipay.mobile.mars;

import android.support.annotation.NonNull;

import com.alipay.mobile.mars.adapter.LogUtil;

public class MarsInitModel {

    public float mDuration; // 单位：秒
    public String mVersion;
    public String mRootPath;
    public boolean mLooping;
    public String mSourceId;
    public String mSource;

    public MarsInitModel(String rootPath, String version, boolean looping, float duration, String sourceId, String source) {
        mRootPath = rootPath;
        mVersion = version;
        mLooping = looping;
        mDuration = duration;
        mSourceId = sourceId;
        mSource = source;
    }

    public String getFormattedSourceId() {
        try {
            if (mSourceId.length() > 32 && mSourceId.contains("A*")) {
                int idx = mSourceId.lastIndexOf("A*");
                return mSourceId.substring(idx, Math.min(idx + 32, mSourceId.length()));
            }
        } catch (Exception e) {
            LogUtil.error("InitModel", e.getMessage());
        }
        return mSourceId;
    }

    @NonNull
    @Override
    public String toString() {
        return super.toString();
    }
}
