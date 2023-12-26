package com.alipay.mobile.mars.adapter;

import android.os.Handler;
import android.os.Looper;

import java.util.concurrent.ArrayBlockingQueue;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

public class TaskScheduleUtil {

    private static Handler sHandler;

    /**
     * 抛任务到UI线程执行
     * @param r 任务
     * @param delay 延迟时间，单位ms
     */
    public static synchronized void postToUiThreadDelayed(Runnable r, long delay) {
        if (sHandler == null) {
            sHandler = new Handler(Looper.getMainLooper());
        }
        sHandler.postDelayed(r, delay);
    }

    /**
     * 抛任务到非UI线程（normal）执行
     * @param r 任务
     */
    public static synchronized void postToNormalThread(Runnable r) {
        if (executor == null) {
            executor = new ThreadPoolExecutor(2, 2, 1, TimeUnit.MINUTES, new ArrayBlockingQueue<Runnable>(128),
                    new ThreadFactory() {
                        @Override
                        public Thread newThread(Runnable runnable) {
                            Thread t = new Thread(runnable);
                            t.setName("Mars_OpenSDK_NormalThread_" + Integer.toString(++threadId));
                            return t;
                        }
                    });
            executor.allowCoreThreadTimeOut(true);
        }
        try {
            executor.execute(r);
        } catch (Exception e) {
            e.printStackTrace();
            LogUtil.error("TaskScheduleUtil", "postToNormalThread error");
        }
    }

    private static ThreadPoolExecutor executor;

    private static int threadId = 0;
    
}
