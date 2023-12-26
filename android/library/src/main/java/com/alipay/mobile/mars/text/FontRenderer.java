package com.alipay.mobile.mars.text;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Color;
import android.graphics.Rect;
import android.text.TextPaint;
import android.text.TextUtils;

import com.alipay.mobile.mars.player.data.MarsDataBase;
import com.alipay.mobile.mars.adapter.LogUtil;

import java.util.ArrayList;
import java.util.HashMap;

public class FontRenderer {

    private static final String TAG = "FontRenderer";

    private FontManager mFontMgr;

    public boolean loadFonts(ArrayList<MarsDataBase.FontInfo> fonts, HashMap<String, String> fontFiles) {
        mFontMgr = new FontManager();

        try {
            for (int i = 0; i < fonts.size(); i++) {
                MarsDataBase.FontInfo font = fonts.get(i);
                String filePath = null;
                if (!TextUtils.isEmpty(font.url)) {
                    filePath = fontFiles.get(font.url);
                }
                if (!mFontMgr.loadFont(font.family, filePath, font.size, font.weight, font.style != 0, font.letterSpace)) {
                    return false;
                }
            }
        } catch (Exception e) {
            LogUtil.error(TAG, "loadFonts.e:" + e.getMessage());
            return false;
        }
        LogUtil.debug(TAG, "loadFonts success");
        return true;
    }

    public Bitmap drawTextOnBitmap(MarsDataBase.TextInfo textInfo, Bitmap bitmap) {
        try {
            if (bitmap == null) {
                LogUtil.error(TAG, "drawTextOnBitmap bitmap is null");
                return null;
            }

            // 创建模版指定大小的透明context
            float imgWidth = textInfo.width;
            float imgHeight = textInfo.height;
            Bitmap newBmp = Bitmap.createBitmap((int) imgWidth, (int) imgHeight, Bitmap.Config.ARGB_8888);
            Canvas canvas = new Canvas(newBmp);
            // 绘制背景，直接拉伸
            canvas.drawBitmap(bitmap,
                    new Rect(0, 0, bitmap.getWidth(), bitmap.getHeight()),
                    new Rect(0, 0, (int) imgWidth, (int) imgHeight),
                    null);

            ArrayList<MarsDataBase.TextContent> texts = textInfo.texts;
            for (int i = 0; i < texts.size(); i++) {
                MarsDataBase.TextContent content = texts.get(i);

                RenderData renderData = new RenderData();
                renderData.setup(content.text, content, mFontMgr);

                // 画调试背景
//                Paint debugPaint = new Paint();
//                debugPaint.setColor(Color.argb(80, 255, 0, 0));
//                canvas.drawRect(new RectF(content.x, content.y, content.x + content.maxWidth, content.y + renderData.lineHeight), debugPaint);

                TextPaint paint = mFontMgr.getTextPaintAt(content.fontIdx);
                paint.setColor(convertColor(content.color));

                canvas.save();
                // 单个文字偏移
                canvas.translate(content.x, renderData.lineHeight + content.y);
                // 处理align
                if (content.align == 1) {
                    canvas.translate((content.maxWidth - renderData.boundWidth) / 2, 0);
                } else if (content.align == 2) {
                    canvas.translate(content.maxWidth - renderData.boundWidth, 0);
                }

                if (renderData.ellipsis != null) {
                    if (content.align == 1 || content.align == 2) {
                        canvas.translate(-renderData.ellipsis.boundWidth, 0);
                        renderData.ellipsis.render(canvas);
                        canvas.translate(renderData.ellipsis.boundWidth, 0);
                    }
                }
                renderData.render(canvas);
                if (renderData.ellipsis != null) {
                    if (content.align != 2) {
                        canvas.translate(renderData.boundWidth, 0);
                        renderData.ellipsis.render(canvas);
                    }
                }

                canvas.restore();
                bitmap.recycle();
            }

            return newBmp;
        } catch (Exception e) {
            LogUtil.error(TAG, "drawTextOnBitmap.e:" + e.getMessage());
        }
        return null;
    }

    private static int convertColor(int rgba) {
        int r, g, b, a;
        a = rgba & 0xff;
        rgba >>= 8;
        b = rgba & 0xff;
        rgba >>= 8;
        g = rgba & 0xff;
        rgba >>= 8;
        r = rgba & 0xff;
        return Color.argb(a, r, g, b);
    }

    private static class RenderData {
        public float ascent;
        public float lineHeight;
        public float boundWidth;

        public RenderData ellipsis = null;

        private String mRenderStr;
        private float mLetterSpace;
        private MarsDataBase.TextContent mContent;
        private FontManager mFontManager;

        void setup(String str, MarsDataBase.TextContent content, FontManager fontManager) {
            setup(str, content, fontManager, true);
        }

        void setup(String str, MarsDataBase.TextContent content, FontManager fontManager, boolean shouldClip) {
            mRenderStr = str;
            mFontManager = fontManager;

            // 处理letterSpace
            mLetterSpace = fontManager.getLetterSpaceAt(content.fontIdx);

            mContent = content;
            boundWidth = fontManager.getTextPaintAt(content.fontIdx).measureText(str) + mLetterSpace * (str.length() - 1);
            ascent = fontManager.getAscent(content.fontIdx);
            lineHeight = fontManager.getTextPaintAt(content.fontIdx).getTextSize();

            if (content.maxWidth > 0 && boundWidth > content.maxWidth && shouldClip) {
                float maxWidth = content.maxWidth;
                if (content.overflow == 2) {
                    ellipsis = new RenderData();
                    ellipsis.setup(content.ellipsisText, content, fontManager, false);
                    // maxWidth减去ellipsis宽度
                    maxWidth -= ellipsis.boundWidth;
                    if (content.align == 1) {
                        // 居中两端都有ellipsis
                        maxWidth -= ellipsis.boundWidth;
                    }
                }

                // 如果ellipsis宽度大于等于maxWidth，直接用ellipisis
                if (maxWidth <= 0) {
                    mRenderStr = "";
                } else {
                    if (content.align == 1) {
                        clipForAlignCenter(maxWidth);
                    } else if (content.align == 2) {
                        clipForAlignRight(maxWidth);
                    } else {
                        clipForAlignLeft(maxWidth);
                    }
                }
            }
        }

        void render(Canvas canvas) {
            if (TextUtils.isEmpty(mRenderStr)) {
                return;
            }
            TextPaint paint = mFontManager.getTextPaintAt(mContent.fontIdx);
            if (mLetterSpace == 0) {
                canvas.drawText(mRenderStr, 0, 0, paint);
            } else {
                canvas.drawText(mRenderStr.substring(0, 1), 0, 0, paint);
                for (int i = 1; i < mRenderStr.length(); i++) {
                    float offsetX = paint.measureText(mRenderStr.substring(0, i)) + mLetterSpace * i;
                    String ch = mRenderStr.substring(i, i + 1);
                    canvas.drawText(ch, offsetX, 0, paint);
                }
            }
        }

        private void clipForAlignCenter(float maxWidth) {
            while (boundWidth > maxWidth && mRenderStr.length() >= 2) {
                mRenderStr = mRenderStr.substring(1, mRenderStr.length() - 1);
                updateBoundWidth();
            }
            if (boundWidth > maxWidth && mRenderStr.length() >= 1) {
                mRenderStr = mRenderStr.substring(1);
                updateBoundWidth();
            }
        }

        private void clipForAlignLeft(float maxWidth) {
            while (boundWidth > maxWidth && mRenderStr.length() >= 1) {
                mRenderStr = mRenderStr.substring(0, mRenderStr.length() - 1);
                updateBoundWidth();
            }
        }

        private void clipForAlignRight(float maxWidth) {
            while (boundWidth > maxWidth && mRenderStr.length() >= 1) {
                mRenderStr = mRenderStr.substring(1);
                updateBoundWidth();
            }
        }

        private void updateBoundWidth() {
            boundWidth = mFontManager.getTextPaintAt(mContent.fontIdx).measureText(mRenderStr)
                    + mLetterSpace * (mRenderStr.length() - 1);
        }
    }

}
