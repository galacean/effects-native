package com.alipay.mobile.mars.player;

import android.graphics.Bitmap;
import android.text.TextUtils;

import com.alipay.mobile.mars.player.data.MarsDataBase;
import com.alipay.mobile.mars.text.FontRenderer;
import com.alipay.mobile.mars.util.CommonUtil;
import com.alipay.mobile.mars.adapter.LogUtil;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class ImageProcessor {

    private static final String TAG = "ImageProcessor";

    private HashMap<String, DataInfo> mImageDataInfos;

    // 原始加载图片列表，用于把文字的imgIdx和imageDataMap对应起来
    private ArrayList<MarsNativeResourceLoader.MarsNativeImageResource> mResList;

    private ArrayList<MarsDataBase.TextInfo> mTexts;

    private ArrayList<MarsDataBase.FontInfo> mFonts;

    // <字体url, 字体路径>
    private HashMap<String, String> mFontDatas;

    ImageProcessor(ArrayList<MarsNativeResourceLoader.MarsNativeImageResource> resList) {
        try {
            mImageDataInfos = new HashMap<>();
            if (resList == null) {
                return;
            }
            mResList = resList;
            for (int i = 0; i < resList.size(); i++) {
                MarsNativeResourceLoader.MarsNativeImageResource res = resList.get(i);
                DataInfo info = new DataInfo();
                if (res.isVideo) {
                    info.isVideo = true;
                    info.videoRes = res.videoRes;
                } else if (CommonUtil.isKtxImageData(res.data)) {
                    info.isKtx = true;
                    info.bytes = res.data;
                } else{
                    info.isKtx = false;
                    if (res.bitmap != null) {
                        info.bitmap = res.bitmap;
                        info.isExternalBitmap = true;
                    } else {
                        info.bitmap = CommonUtil.decodeImage(res.data);
                    }
                }
                mImageDataInfos.put(res.key, info);
            }
        } catch (Exception e) {
            LogUtil.error(TAG, "ImageProcessor.e:" + e.getMessage());
            mImageDataInfos = null;
        }
    }

    void setTexts(ArrayList<MarsDataBase.TextInfo> texts, ArrayList<MarsDataBase.FontInfo> fonts) {
        mTexts = texts;
        mFonts = fonts;
    }

    void setFontDataMap(HashMap<String, String> fontDataMap) {
        mFontDatas = fontDataMap;
    }

    Map<String, DataInfo> process(Map<String, String> variables) {
        try {
            if (mTexts != null && !mTexts.isEmpty()) {
                // 有文字，先把文字画到图片上
                FontRenderer fontRenderer = new FontRenderer();
                if (!fontRenderer.loadFonts(mFonts, mFontDatas)) {
                    return null;
                }
                for (int i = 0; i < mTexts.size(); i++) {
                    MarsDataBase.TextInfo textInfo = mTexts.get(i);
                    String imgUrl = mResList.get(textInfo.imgIdx).key;
                    if (TextUtils.isEmpty(imgUrl)) {
                        LogUtil.error(TAG, "fail to find imgUrl for " + textInfo.imgIdx);
                        return null;
                    }
                    DataInfo info = mImageDataInfos.get(imgUrl);
                    if (info == null) {
                        LogUtil.error(TAG, "fail to find image for " + imgUrl);
                        return null;
                    }
                    if (info.isKtx) {
                        LogUtil.error(TAG, "can't draw text on ktx " + imgUrl);
                        return null;
                    }
                    // 填充文字动态数据
                    if (variables != null) {
                        for (int j = 0; j < textInfo.texts.size(); j++) {
                            MarsDataBase.TextContent content = textInfo.texts.get(j);
                            if (!TextUtils.isEmpty(content.name) && variables.containsKey(content.name)) {
                                content.text = variables.get(content.name);
                            }
                        }
                    }
                    info.bitmap = fontRenderer.drawTextOnBitmap(textInfo, info.bitmap);
                    if (info.bitmap == null) {
                        LogUtil.error(TAG, "fail to draw text on ktx " + imgUrl);
                        return null;
                    }
                }
            }

            return mImageDataInfos;
        } catch (Exception e) {
            LogUtil.error(TAG, "process.e:" + e.getMessage());
            return null;
        }
    }

    void releaseData() {
        try {
            for (String key : mImageDataInfos.keySet()) {
                DataInfo info = mImageDataInfos.get(key);
                if (info != null && info.bitmap != null) {
                    if (!info.isExternalBitmap) {
                        info.bitmap.recycle();
                    }
                    info.bitmap = null;
                }
            }
        } catch (Exception e) {
            LogUtil.error(TAG, "releaseData.e:" + e.getMessage());
        }
    }

    public static class DataInfo {
        boolean isKtx = false;
        boolean isVideo = false;
        byte[] bytes = null;
        MarsNativeResourceLoader.MarsNativeVideoResource videoRes = null;
        Bitmap bitmap = null;
        boolean isExternalBitmap = false; // 区分bitmap来源
    }
}
