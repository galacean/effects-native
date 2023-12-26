package com.alipay.mobile.mars.player.data;

import android.graphics.Point;
import android.os.Build;
import android.text.TextUtils;

import com.alipay.mobile.mars.adapter.ContextUtil;
import com.alipay.mobile.mars.util.CommonUtil;
import com.alipay.mobile.mars.util.Constants;
import com.alipay.mobile.mars.adapter.LogUtil;

import java.io.File;
import java.util.ArrayList;
import java.util.List;

public abstract class MarsDataBase {

    private final static String TAG = "MarsDataBase";

    // json+bin
    private final static String FILE_MARS_JSON = "mars.json";
    private final static String FILE_MARS_BIN = "mars.bin";

    private String mDirPath;

    protected String mVersion; // 数据版本（for客户端）

    protected Point mPreviewSize; // 为了获取aspect

    protected float mDuration; // 持续时间

    protected ArrayList<ImageInfo> mImages; // 图片列表

    protected ArrayList<TextInfo> mTexts; // 文字列表
    protected ArrayList<FontInfo> mFonts; // 字体列表

    protected boolean mValid = false; // 是否是合法的数据

    protected MarsDataBase(String dirPath) {
        mDirPath = dirPath;
    }

    // 获取数据格式类型
    public abstract Type getType();

    public boolean isValid() {
        return mValid;
    }

    public String getDirPath() {
        return mDirPath;
    }

    public float getAspect() {
        if (mPreviewSize == null) {
            // mPreviewSize没有初始化
            return 1;
        }
        float w = mPreviewSize.x;
        float h = mPreviewSize.y;
        if (w == 0 || h == 0) {
            return 1;
        }
        return w / h;
    }

    public Point getPreviewSize() {
        return mPreviewSize;
    }

    public float getDuration() {
        return mDuration;
    }

    public List<ImageInfo> getImages() {
        return mImages;
    }

    public ArrayList<TextInfo> getTexts() {
        return mTexts;
    }

    public ArrayList<FontInfo> getFonts() {
        return mFonts;
    }

    public static MarsDataBase createMarsData(final String dirPath) throws Exception {
        try {
            if (TextUtils.isEmpty(dirPath)) {
                // 数据目录为空
                throw new Exception("createMarsData:dirPath empty");
            }
            String jsonPath = null;
            String binPath = null;
            if (dirPath.startsWith(Constants.RES_PATH_ASSETS_PREFIX)) {
                // 获取assets目录下的文件列表
                String[] fileList = CommonUtil.readAssetsDir(
                        ContextUtil.getApplicationContext(),
                        dirPath.substring(Constants.RES_PATH_ASSETS_PREFIX.length()));
                if (fileList == null) {
                    // 数据目录不存在或不是目录
                    throw new Exception("createMarsData:dirPath invalid," + dirPath);
                }
                // 遍历列表，看有没有对应的文件
                for (String fileName : fileList) {
                    if (fileName.equals(FILE_MARS_JSON)) {
                        jsonPath = dirPath + File.separator + FILE_MARS_JSON;
                    } else if (fileName.equals(FILE_MARS_BIN)) {
                        binPath = dirPath + File.separator + FILE_MARS_BIN;
                    }
                }
            } else {
                // 资源在磁盘上
                File dir = new File(dirPath);
                if (!dir.exists() || !dir.isDirectory()) {
                    // 数据目录不存在或不是目录
                    throw new Exception("createMarsData:dirPath invalid," + dir.exists());
                }
                File marsJson = new File(dir, FILE_MARS_JSON);
                if (marsJson.exists()) {
                    LogUtil.debug(TAG, "createMarsData:mars.json exist");
                    File marsBin = new File(dir, FILE_MARS_BIN);
                    if (!marsBin.exists()) {
                        throw new Exception("createMarsData:mars.bin not exist");
                    }
                    // 存在mars.json和mars.bin
                    jsonPath = marsJson.getAbsolutePath();
                    binPath = marsBin.getAbsolutePath();
                }
            }

            if (jsonPath != null) {
                if (binPath != null) {
                    return new MarsDataJsonBin(jsonPath, binPath, dirPath);
                }
            }
            throw new Exception("createMarsData:invalid");
        } catch (Exception e) {
            LogUtil.error(TAG, "createMarsData..e:" + e.getMessage());
            throw new Exception(e.getMessage());
        }
    }

    public static boolean deviceSupportWebp() {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.O) {
            return false;
        }
        return true;
    }

    public static class ImageInfo {
        public String url = null;
        public String astc = null;

        public int templateIdx = -1; // 如果是数据模版，这里指向要替换的图片

        public boolean isVideo = false;
        public boolean isTransparentVideo = false;
    }

    public static class TextContent {
        public float x;
        public float y;
        public String name;
        public String text;
        public float maxWidth;
        public int fontIdx;
        public int align;
        public int color;
        public int overflow;
        public String ellipsisText;
    }

    public static class TextInfo {
        public int imgIdx;
        public float x; // 暂时无用
        public float y; // 暂时无用
        public float width;
        public float height;
        public ArrayList<TextContent> texts;
    }

    public static class FontInfo {
        public String url;
        public int weight;
        public float size;
        public float letterSpace;
        public String family;
        public int style;
    }

    public enum Type {
        JSON_BIN,
    }

}
