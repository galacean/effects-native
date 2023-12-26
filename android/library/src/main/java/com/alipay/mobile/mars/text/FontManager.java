package com.alipay.mobile.mars.text;

import android.graphics.Paint;
import android.graphics.Typeface;
import android.os.Build;
import android.text.TextPaint;
import android.text.TextUtils;

import com.alipay.mobile.mars.adapter.LogUtil;

import java.util.ArrayList;

public class FontManager {

    private final ArrayList<FontHolder> mFontHolders = new ArrayList<>();

    boolean loadFont(String fontName, String filePath, float size, int weight, boolean italic, float letterSpace) {
        try {
            FontHolder holder = new FontHolder(size, weight, italic, letterSpace);
            if (TextUtils.isEmpty(filePath)) {
                holder.setFontFamily(fontName);
            } else {
                holder.setFontPath(filePath);
            }

            mFontHolders.add(holder);
        } catch (Exception e) {
            LogUtil.error("FontManager", "loadFont.e:" + e.getMessage());
            return false;
        }
        return true;
    }

    TextPaint getTextPaintAt(int idx) {
        return mFontHolders.get(idx).mPaint;
    }

    float getLetterSpaceAt(int idx) {
        return mFontHolders.get(idx).mLetterSpace;
    }

    float getAscent(int idx) {
        return -mFontHolders.get(idx).mPaint.getFontMetrics().ascent;
    }

    private static class FontHolder {
        private Typeface mBaseTypeface;
        private Typeface mTypeface;
        private final TextPaint mPaint;

        private final int mWeight;

        private final float mLetterSpace;

        FontHolder(float size, int weight, boolean italic, float letterSpace) {
            mPaint = new TextPaint();
            mPaint.setTextSize(size);
            mPaint.setAntiAlias(true);
            if (italic) {
                // 斜体统一使用skew实现
                mPaint.setTextSkewX(-0.24f);
            }
            mPaint.setStyle(Paint.Style.FILL);
            mWeight = weight;
            if (weight > 450) {
                mPaint.setFakeBoldText(true);
            }

            mLetterSpace = letterSpace;
        }

        void setFontFamily(String familyName) {
            mBaseTypeface = Typeface.create(familyName, Typeface.NORMAL);
            updateTypeFace();
        }

        void setFontPath(String path) {
            mBaseTypeface = Typeface.createFromFile(path);
            updateTypeFace();
        }

        private void updateTypeFace() {
            if (mWeight < 350 && Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
                mTypeface = Typeface.create(mBaseTypeface, 300, false);
            } else {
                mTypeface = mBaseTypeface;
            }
            mPaint.setTypeface(mTypeface);
        }
    }

}
