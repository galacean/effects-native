package com.alipay.mobile.mars.player.data;

import android.content.Context;
import android.graphics.Point;

import com.alibaba.fastjson.JSONArray;
import com.alibaba.fastjson.JSONObject;
import com.alipay.mobile.mars.util.IOUtil;
import com.alipay.mobile.mars.MarsNativeConstants;
import com.alipay.mobile.mars.adapter.ContextUtil;
import com.alipay.mobile.mars.util.CommonUtil;
import com.alipay.mobile.mars.util.Constants;
import com.alipay.mobile.mars.adapter.LogUtil;

import java.io.FileInputStream;
import java.io.InputStream;
import java.util.ArrayList;

public class MarsDataJsonBin extends MarsDataBase {

    private final static String TAG = "MarsDataJsonBin";

    private String mBinFilePath;

    private byte[] mBinBytes = null;

    MarsDataJsonBin(String jsonFilePath, String binFilePath, String dirPath) throws Exception {
        super(dirPath);
        if (!loadJson(jsonFilePath)) {
            throw new Exception("loadJson fail");
        }
        if (binFilePath.startsWith(Constants.RES_PATH_ASSETS_PREFIX)) {
            // 如果是assets下的文件，需要读出二进制流
            mBinBytes = CommonUtil.readAssetsBinary(
                    ContextUtil.getApplicationContext(),
                    binFilePath.substring(MarsNativeConstants.PATH_ASSETS_PREFIX.length())
            );
            if (mBinBytes == null) {
                throw new Exception("readAssetsBinary fail");
            }
        } else {
            mBinFilePath = binFilePath;
        }
        mValid = true;
    }

    public String getBinFilePath() {
        return mBinFilePath;
    }

    public byte[] getBinBytes() {
        return mBinBytes;
    }

    private boolean loadJson(String jsonFilePath) {
        InputStream is = null;
        boolean result = false;
        try {
            do {
                if (jsonFilePath.startsWith(Constants.RES_PATH_ASSETS_PREFIX)) {
                    // assets目录下的文件
                    Context context = ContextUtil.getApplicationContext();
                    is = context.getAssets().open(jsonFilePath.substring(MarsNativeConstants.PATH_ASSETS_PREFIX.length()));
                } else {
                    // 磁盘上的文件
                    is = new FileInputStream(jsonFilePath);
                }
                byte[] bytes = CommonUtil.readFileStreamBinaryAndClose(is);
                if (bytes == null || bytes.length == 0) {
                    // 文件为空
                    LogUtil.error(TAG, "loadJson:empty bytes");
                    break;
                }
                String jsonStr = new String(bytes);

                JSONObject scene = JSONObject.parseObject(jsonStr);
                mVersion = scene.getString("version");

                JSONObject meta = scene.getJSONObject("meta");
                JSONArray previewSize = meta.getJSONArray("previewSize");
                mPreviewSize = new Point(previewSize.getIntValue(0), previewSize.getIntValue(1));

                mDuration = scene.getFloatValue("duration");

                mImages = new ArrayList<>();
                JSONArray images = scene.getJSONArray("images");
                LogUtil.debug(TAG, "images count " + images.size());

                for (int i = 0; i < images.size(); i++) {
                    JSONObject image = images.getJSONObject(i);
                    ImageInfo info = new ImageInfo();
                    info.url = image.getString("url");
                    if (deviceSupportWebp() && image.containsKey("webp")) {
                        String webp = image.getString("webp");
                        if (webp.length() != 0) {
                            // 如果存在webp，使用webp
                            info.url = webp;
                        }
                    }
                    if (image.containsKey("isVideo")) {
                        info.isVideo = image.getBooleanValue("isVideo");
                    }
                    if (info.isVideo) {
                        if (image.containsKey("isTransparent")) {
                            info.isTransparentVideo = image.getBooleanValue("isTransparent");
                        }
                    }
                    if (image.containsKey("templateIdx")) {
                        info.templateIdx = image.getIntValue("templateIdx");
                    }
                    // 默认开启压缩纹理
                    if (image.containsKey("compressed")) {
                        JSONObject compressed = image.getJSONObject("compressed");
                        if (compressed.containsKey("astc")) {
                            info.astc = compressed.getString("astc");
                            if (info.astc.length() == 0) {
                                info.astc = null;
                            }
                        }
                    }
                    mImages.add(info);
                }

                JSONObject marsTexts = scene.getJSONObject("marsTexts");
                if (marsTexts != null) {
                    JSONArray textsData = marsTexts.getJSONArray("texts");
                    JSONArray fontsData = marsTexts.getJSONArray("fonts");
                    ArrayList<TextInfo> texts = new ArrayList<>();
                    ArrayList<FontInfo> fonts = new ArrayList<>();

                    for (int i = 0; i < textsData.size(); i++) {
                        JSONObject data = textsData.getJSONObject(i);
                        TextInfo text = new TextInfo();
                        text.imgIdx = data.getIntValue("imgIdx");
                        text.x = data.getFloatValue("x");
                        text.y = data.getFloatValue("y");
                        text.width = data.getFloatValue("width");
                        text.height = data.getFloatValue("height");

                        JSONArray contentsData = data.getJSONArray("texts");
                        ArrayList<TextContent> contents = new ArrayList<>();
                        for (int j = 0; j < contentsData.size(); j++) {
                            JSONArray contentData = contentsData.getJSONArray(j);
                            TextContent content = new TextContent();
                            content.x = contentData.getFloatValue(0);
                            content.y = contentData.getFloatValue(1);
                            content.name = contentData.getString(2);
                            content.text = contentData.getString(3);
                            content.maxWidth = contentData.getFloatValue(4);
                            content.fontIdx = contentData.getIntValue(5);
                            content.align = contentData.getIntValue(6);
                            content.color = contentData.getIntValue(7);
                            content.overflow = contentData.getIntValue(8);
                            content.ellipsisText = contentData.getString(9);
                            contents.add(content);
                        }

                        text.texts = contents;
                        texts.add(text);
                    }
                    for (int i = 0; i < fontsData.size(); i++) {
                        JSONObject data = fontsData.getJSONObject(i);
                        FontInfo font = new FontInfo();
                        font.url = data.getString("url");
                        font.weight = data.getIntValue("weight");
                        font.size = data.getFloatValue("size");
                        font.letterSpace = data.getFloatValue("letterSpace");
                        font.family = data.getString("family");
                        font.style = data.getIntValue("style");
                        fonts.add(font);
                    }

                    if (fonts.size() == 0) {
                        break;
                    }
                    mFonts = fonts;
                    mTexts = texts;
                }

                result = true;
            } while (false);
        } catch (Exception e) {
            LogUtil.error(TAG, "loadJson..e:" + e.getMessage());
        } finally {
            if (is != null) {
                IOUtil.closeStream(is);
            }
        }
        return result;
    }

    @Override
    public Type getType() {
        return Type.JSON_BIN;
    }

}
