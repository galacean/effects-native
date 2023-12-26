package com.alipay.mobile.mars.player.extensions;

import com.alipay.mobile.mars.player.MarsNativePlayer;

import java.util.ArrayList;

public abstract class MarsPositionPathModifierExtension implements MarsNativePlayer.MarsNativeExtension {

    private final ArrayList<String> mItemNames;

    private long mModifierPtr = 0;

    /**
     * @param params 修改path的item列表
     */
    public MarsPositionPathModifierExtension(ArrayList<String> params) {
        mItemNames = new ArrayList<>();
        mItemNames.addAll(params);
    }

    /**
     * 业务自行实现，获取动画相关参数
     */
    protected abstract void onSceneDataParams(
            float pixelRatio, float aspect, float animWidth, float animHeight);

    /**
     * 业务自行实现，修改path数据
     * @param type type当前只有bezier曲线
     */
    protected abstract MarsPositionPathModifyResult modifyItemPathData(
            String itemName, String type, float[] value, float[] value2);

    @Override
    public boolean onSceneDataCreated(long sceneDataPtr, String[] errMsg) {
        mModifierPtr = CreateExtPositionPathModifier();
        if (!ExtPositionPathModifierSetSceneData(mModifierPtr, sceneDataPtr)) {
            if (errMsg != null) {
                errMsg[0] = "SetSceneData fail";
            }
            return false;
        }
        float[] animParams = ExtPositionPathModifierGetParams(mModifierPtr);
        onSceneDataParams(animParams[0], animParams[1], animParams[2], animParams[3]);
        for (int i = 0; i < mItemNames.size(); i++) {
            String name = mItemNames.get(i);
            float[][] dataArr = ExtPositionPathModifierGetPositionPathByName(mModifierPtr, name);
            if (dataArr == null) {
                if (errMsg != null) {
                    errMsg[0] = "GetPositionPathByName fail";
                }
                return false;
            }
            float[] data = dataArr[0];
            float[] data2 = dataArr[1];
            MarsPositionPathModifyResult result = modifyItemPathData(
                    name, data2 != null ? "bezier" : "vec3", data, data2);
            if (result == null) {
                if (errMsg != null) {
                    errMsg[0] = "modifyItemPathData fail";
                }
                return false;
            }

            ExtPositionPathModifierModifyPositionPath(mModifierPtr, name, result.data, result.data2);
        }
        return true;
    }

    @Override
    public long getCustomPlugin(String[] name) {
        return 0;
    }

    @Override
    public void onDestroy() {
        if (mModifierPtr != 0) {
            ExtPositionPathModifierDestroy(mModifierPtr);
            mModifierPtr = 0;
        }
    }

    public static class MarsPositionPathModifyResult {
        public float[] data;
        public float[] data2;
    }

    private native long CreateExtPositionPathModifier();

    private native boolean ExtPositionPathModifierSetSceneData(long modifier, long sceneData);

    /**
     * @return [pixelRatio, aspect, animWidth, animHeight]
     */
    private native float[] ExtPositionPathModifierGetParams(long modifier);

    private native float[][] ExtPositionPathModifierGetPositionPathByName(long modifier, String name);

    private native void ExtPositionPathModifierModifyPositionPath(long modifier, String name, float[] data, float[] data2);

    private native void ExtPositionPathModifierDestroy(long modifier);
}
