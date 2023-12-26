package com.alipay.mobile.mars.view;

import android.annotation.SuppressLint;
import android.content.Context;
import android.graphics.SurfaceTexture;
import android.os.Build;
import android.view.Surface;
import android.view.TextureView;
import android.view.ViewGroup;
import android.widget.FrameLayout;

import com.alipay.mobile.mars.adapter.LogUtil;

import java.lang.ref.WeakReference;

public class MarsTextureView extends TextureView implements TextureView.SurfaceTextureListener {
    private static final String TAG = "MarsTextureView";

    private WeakReference<MarsSurfaceListener> weakCallback;

    private SurfaceTexture mCacheSurfaceTex = null;

    private static int sIdx = 0;

    private int mIdx;

    private boolean mPreserveSurfaceOnDetach = false;

    public MarsTextureView(Context context, MarsSurfaceListener callback) {
        super(context);
        mIdx = ++sIdx;
        weakCallback = new WeakReference<>(callback);
        setSurfaceTextureListener(this);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN) {
            mPreserveSurfaceOnDetach = true;
        }

        setLayoutParams(new FrameLayout.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));
        setOpaque(false);
    }

    @SuppressLint("NewApi")
    @Override
    public void onSurfaceTextureAvailable(SurfaceTexture surface, int width, int height) {
        LogUtil.debug(TAG, "onSurfaceTextureAvailable " + mIdx + " width:" + width + " height:" + height);
        MarsSurfaceListener callback = weakCallback.get();

        Surface surfaceObject;
        if (mPreserveSurfaceOnDetach && mCacheSurfaceTex != null) {
            setSurfaceTexture(mCacheSurfaceTex);
            surfaceObject = new Surface(mCacheSurfaceTex);
        } else {
            surfaceObject = new Surface(surface);
        }
        if (callback != null) {
            callback.onSurfaceCreated(surfaceObject);
        }
        surfaceObject.release();
    }

    @Override
    public void onSurfaceTextureSizeChanged(SurfaceTexture surface, int width, int height) {
        LogUtil.debug(TAG, "onSurfaceTextureSizeChanged");
        MarsSurfaceListener callback = weakCallback.get();
        if (callback != null) {
            callback.onSurfaceResize(width, height);
        }
    }

    @Override
    public boolean onSurfaceTextureDestroyed(SurfaceTexture surface) {
        LogUtil.debug(TAG, "onSurfaceTextureDestroyed" + mIdx);
        MarsSurfaceListener callback = weakCallback.get();

        if (mPreserveSurfaceOnDetach) {
            mCacheSurfaceTex = surface;
        }
        if (callback != null) {
            callback.onSurfaceDestroyed();
        }
        return !mPreserveSurfaceOnDetach;
    }

    @Override
    public void onSurfaceTextureUpdated(SurfaceTexture surface) {
//        LogUtil.debug(TAG, "onSurfaceTextureUpdated");
    }

    public interface MarsSurfaceListener {
        void onSurfaceCreated(Surface surface);

        void onSurfaceResize(int width, int height);

        void onSurfaceDestroyed();
    }

    @Override
    protected void finalize() throws Throwable {
        LogUtil.debug(TAG, "MarsTextureView finalize");
        super.finalize();
        if (mCacheSurfaceTex != null) {
//            mCacheSurfaceTex.release();
            mCacheSurfaceTex = null;
        }
    }
}
