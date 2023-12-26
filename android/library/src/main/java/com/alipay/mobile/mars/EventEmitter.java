package com.alipay.mobile.mars;

import android.graphics.Bitmap;

import com.alipay.mobile.mars.util.CommonUtil;
import com.alipay.mobile.mars.adapter.LogUtil;

import java.lang.ref.WeakReference;
import java.util.HashMap;

public class EventEmitter {
    private static HashMap<Integer, WeakReference<EventListener>> sListeners = new HashMap<>();

    private static final Object sMutex = new Object();

    // called from c++
    public static void onEvent(int playerId, int type, String msg) {
        LogUtil.debug("EventEmitter", playerId + " " + type + " " + msg);
        synchronized (sMutex) {
            if (sListeners.containsKey(playerId)) {
                WeakReference<EventListener> weakReference = sListeners.get(playerId);
                EventListener listener = weakReference.get();
                if (listener != null) {
                    listener.onEvent(type, msg);
                    return;
                }
            }
            LogUtil.error("EventEmitter", "listener not found");
        }
    }

    // called from c++
    public static void onLog(int level, String msg) {
        try {
            if (level == 4) {
                LogUtil.error("clog", msg);
            } else {
                LogUtil.debug("clog", msg);
            }
        } catch (Exception e) {
            // 这里没有什么兜底方案了
        }
    }

    // called from c++
    public static Bitmap parseBitmap(byte[] buffer) {
        return CommonUtil.decodeImage(buffer);
    }

    public static void registerListener(int idx, EventListener listener) {
        synchronized (sMutex) {
            sListeners.put(idx, new WeakReference<EventListener>(listener));
        }
    }

    public static void unregisterListener(int idx) {
        synchronized (sMutex) {
            sListeners.remove(idx);
        }
        LogUtil.debug("EventEmitter", "remain listeners:" + sListeners.size());
    }

    public interface EventListener {
        void onEvent(int type, String msg);
    }
}
