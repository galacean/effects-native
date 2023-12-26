package com.alipay.mobile.mars.player;

import android.text.TextUtils;

import com.alipay.mobile.mars.adapter.ConfigUtil;
import com.alipay.mobile.mars.adapter.MonitorUtil;
import com.alipay.mobile.mars.adapter.StorageUtil;
import com.alipay.mobile.mars.adapter.TaskScheduleUtil;
import com.alipay.mobile.mars.adapter.LogUtil;

import java.io.File;

public class CleanUtil {

    private static final String TAG = "CleanUtil";

    // 关闭清理的开关，默认不关闭
    private static final String MN_CLEAN_CLOSE_SWITCH = "mn_Purgeable_Clean_Disable";
    // 检查&文件过期时间间隔
    private static final String MN_CLEAN_INTERVAL_SWITCH = "mn_Purgeable_Date_Diff";

    private static boolean sCleanSwitchClosed = false;
    private static long sCleanInterval = 0;
    private static long sLastCleanTimestamp = 0;

    private static void initIfNeed() {
        if (sCleanInterval == 0) {
            try {
                // 初始化开关值
                sCleanSwitchClosed = "true".equals(ConfigUtil.getStringValue(MN_CLEAN_CLOSE_SWITCH));
                if (!sCleanSwitchClosed) {
                    // 如果没有关闭清理功能，读取上次清理的时间
                    sLastCleanTimestamp = StorageUtil.getLong(StorageUtil.KEY_LAST_CLEAN_TIMESTAMP, 0);
                    if (sLastCleanTimestamp == 0) {
                        // 没有记录，默认从当前时间开始计算
                        sLastCleanTimestamp = System.currentTimeMillis();
                        StorageUtil.putLong(StorageUtil.KEY_LAST_CLEAN_TIMESTAMP, sLastCleanTimestamp);
                    }
                    // 获取清理资源的时间间隔
                    String time = ConfigUtil.getStringValue(MN_CLEAN_INTERVAL_SWITCH);
                    if (!TextUtils.isEmpty(time)) {
                        sCleanInterval = (long) (Float.parseFloat(time) * 1000) * 24 * 60 * 60;
                    }
                }
            } catch (Exception e) {
                sCleanSwitchClosed = true;
                LogUtil.error(TAG, "init.e:" + e.getMessage());
            } finally {
                if (sCleanInterval <= 0) {
                    // 如果cleanInterval拿到的是0，强制设置为3天；
                    sCleanInterval = (long) 3 * 24 * 60 * 60 * 1000;
                }
            }
            LogUtil.debug(TAG, "cleanSwitchClosed=" + sCleanSwitchClosed +
                    ",lastCleanTimestamp=" + sLastCleanTimestamp
                    + ",cleanInterval=" + sCleanInterval);
        }
    }

    static void tryCleanMarsFiles() {
        initIfNeed();
        long currentTime = System.currentTimeMillis();
        if (sCleanSwitchClosed || (currentTime - sLastCleanTimestamp < sCleanInterval)) {
            // 开关关闭或未到清理时间
            LogUtil.error(TAG, "tryCleanMarsFiles skip," + (currentTime - sLastCleanTimestamp)
                    + "," + sCleanInterval);
            return;
        }
        // 抛到子线程去处理
        TaskScheduleUtil.postToNormalThread(new Runnable() {
            @Override
            public void run() {
                try {
                    // 获取mars资源总目录
                    String dirPath = FileOperation.getMarsDir();
                    File dir = new File(dirPath);
                    if (!dir.isDirectory()) {
                        // 不是目录
                        LogUtil.error(TAG, "tryCleanMarsFiles " + dirPath + " is not dir");
                        return;
                    }
                    File[] files = dir.listFiles();
                    if (files == null || files.length == 0) {
                        LogUtil.debug(TAG, "tryCleanMarsFiles " + dirPath + " is empty");
                        return;
                    }
                    String errFile = null;
                    String errMsg = null;
                    for (int i = 0; i < files.length; i++) {
                        File file = files[i];
                        // 对于_tmp目录，会生成一个时间戳文件并返回当前时间戳，不会误删
                        String e = cleanMarsDir(file);
                        if (!TextUtils.isEmpty(e)) {
                            // 存在错误，保存错误的文件和原因。只保留最后一个，不阻塞删除其他文件
                            errMsg = e;
                            errFile = file.getName();
                        }
                    }
                    if (!TextUtils.isEmpty(errMsg)) {
                        // 有异常，上报异常埋点
                        LogUtil.debug(TAG, "tryCleanMarsFiles fail," + errFile + "," + errMsg);
                        MonitorUtil.monitorCleanEvent("clean", "clean", false, errFile, errMsg);
                    } else {
                        // 无异常，上报成功埋点
                        LogUtil.debug(TAG, "tryCleanMarsFiles success");
                        MonitorUtil.monitorCleanEvent("clean", "clean", true);
                    }
                } catch (Exception e) {
                    LogUtil.error(TAG, "tryCleanMarsFiles.e:" + e.getMessage());
                    MonitorUtil.monitorCleanEvent("clean", "clean", false, "code", e.getMessage());
                }
            }
        });
        sLastCleanTimestamp = currentTime;
        StorageUtil.putLong(StorageUtil.KEY_LAST_CLEAN_TIMESTAMP, sLastCleanTimestamp);
    }

    static boolean disabled() {
        return sCleanSwitchClosed;
    }

    private static String cleanMarsDir(File file) {
        LogUtil.debug(TAG, "cleanMars," + file.getName());
        return FileOperation.deleteDirIfExpired(file, sCleanInterval);
    }

}
