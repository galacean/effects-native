package com.alipay.mobile.mars.player.extensions;

import android.support.annotation.Keep;

import com.alipay.mobile.mars.player.MarsNativePlayer;
import com.alipay.mobile.mars.util.JNIUtil;

import java.util.ArrayList;

public abstract class MarsCustomMorphExtension implements MarsNativePlayer.MarsNativeExtension {

    private final ArrayList<String> mParams;

    public MarsCustomMorphExtension(ArrayList<String> params) {
        mParams = new ArrayList<>();
        mParams.addAll(params);
    }

    @Override
    public boolean onSceneDataCreated(long sceneDataPtr, String[] errMsg) {
        float duration = JNIUtil.nativeSceneDataGetDuration(sceneDataPtr);
        onSceneDataParams(duration);
        return true;
    }

    @Override
    public long getCustomPlugin(String[] name) {
        if (name != null) {
            name[0] = "morph";
        }
        String[] params = new String[mParams.size()];
        for (int i = 0; i < mParams.size(); i++) {
            params[i] = mParams.get(i);
        }
        return MarsCustomMorphExtension.this.createCustomMorphPlugin(params);
    }

    @Override
    public void onDestroy() {
    }

    @Keep
    private Object[] generateMorphDataFromNative(String name, float time) {
        return generateMorphData(name, time);
    }

    /**
     * 业务自行实现，获取动画相关参数
     */
    protected void onSceneDataParams(float duration) {
    }

    /**
     * 业务自行实现，动态修改遮罩
     * time: 0 - 1
     */
    protected abstract Object[] generateMorphData(String name, float time);

    private native long createCustomMorphPlugin(String[] params);
}
