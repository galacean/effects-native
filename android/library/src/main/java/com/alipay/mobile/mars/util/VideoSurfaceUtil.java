package com.alipay.mobile.mars.util;

import android.graphics.SurfaceTexture;
import android.support.annotation.Keep;
import android.view.Surface;

import com.alipay.mobile.mars.adapter.LogUtil;

public class VideoSurfaceUtil {

    // called from c++
    @Keep
    public static Object[] createSurface(int tid) {
        try {
            VideoReceiverSurface surface = new VideoReceiverSurface(tid);
            Object[] ret = new Object[2];
            ret[0] = surface;
            ret[1] = surface.getSurface();
            LogUtil.debug("VideoJava", "createSurface " + tid + "," + ret[0] + "," + ret[1]);
            return ret;
        } catch (Throwable e) {
            LogUtil.error("VideoJava", "createSurface.e:" + e.getMessage());
            return null;
        }
    }

    // called from c++
    @Keep
    public static void releaseSurface(Object param1) {
        LogUtil.debug("VideoJava", "releaseSurface " + param1);
        try {
            VideoReceiverSurface surface = (VideoReceiverSurface) param1;
            surface.release();
        } catch (Throwable e) {
            LogUtil.error("VideoJava", "releaseSurface.e:" + e.getMessage());
        }
        LogUtil.debug("VideoJava", "releaseSurface end");
    }

    /**
     *
     * @param param1 VideoReceiverSurface
     * @return
     *  0: 出现异常
     *  1: 有首帧
     *  2: 无异常且无首帧
     */
    // called from c++
    @Keep
    public static int updateTexture(Object param1) {
        try {
            return ((VideoReceiverSurface) param1).updateSurfaceTexture();
        } catch (Throwable e) {
            LogUtil.error("VideoJava", "updateTexture.e:" + e.getMessage());
            return 0;
        }
    }

    private static class VideoReceiverSurface implements SurfaceTexture.OnFrameAvailableListener {
        private static final String TAG = "VideoReceiverSurface";

        private static final int FRAME_WAIT_TIMEOUT_MS = 1000;

        private SurfaceTexture surfaceTexture;
        private Surface surface;

        private final Object frameSyncObject = new Object();
        private boolean frameAvailable;

        VideoReceiverSurface(int tid) {
            surfaceTexture = new SurfaceTexture(tid);
            surface = new Surface(surfaceTexture);
            surfaceTexture.setOnFrameAvailableListener(this);

            LogUtil.debug(TAG, "create " + tid + "," + surface);
        }

        void release() {
            if (surface != null) {
                surface.release();
                surface = null;
            }
            if (surfaceTexture != null) {
                surfaceTexture.release();
                surfaceTexture = null;
            }
        }

        @Override
        public void onFrameAvailable(SurfaceTexture surfaceTexture) {
            synchronized (frameSyncObject) {
//                if (frameAvailable) {
//                    LogUtil.error(TAG, "frameAvailable already set, frame could be dropped");
//                }
                frameAvailable = true;
            }
        }

        public int updateSurfaceTexture() {
            synchronized (frameSyncObject) {
                if (!frameAvailable) {
                    return 2;
                }
                frameAvailable = false;
            }
            surfaceTexture.updateTexImage();
            return 1;
        }

        Surface getSurface() {
            return surface;
        }
    }
}
