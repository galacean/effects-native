package com.alipay.mobile.mars.player;

import android.text.TextUtils;

import com.alipay.mobile.mars.util.IOUtil;
import com.alipay.mobile.mars.adapter.ContextUtil;
import com.alipay.mobile.mars.adapter.MonitorUtil;
import com.alipay.mobile.mars.adapter.DownloadUtil;
import com.alipay.mobile.mars.adapter.LogUtil;
import com.alipay.mobile.mars.adapter.ZipUtil;

import java.io.File;
import java.io.FileInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Map;

public class FileOperation {

    private static final String TAG = "FileOperation";

    private static final Object sLockToken = new Object();

    private static String sMarsDir = null;

    private static final Map<String, ArrayList<DownloadZipCallback>> sLoadingMap = new HashMap<>();

    static boolean checkDirExist(String dirPath, boolean needUpdateTimeStamp, long timeStamp) {
        synchronized (sLockToken) {
            try {
                File dirFile = new File(dirPath);
                // 检查dirPath是否存在并且是目录
                if (dirFile.exists()) {
                    if (dirFile.isDirectory()) {
                        File[] files = dirFile.listFiles();
                        if (files != null && files.length > 0) {
                            // dirPath下面有文件，认为是合法的资源
                            LogUtil.debug(TAG, "ResourceLoader.loadZip:already exist:" + dirPath + "," + files.length);
                            if (needUpdateTimeStamp) {
                                // 更新时间戳
                                updateTimeStampLocked(dirFile, timeStamp);
                            }
                            if (sLoadingMap.isEmpty()) {
                                // 尝试清理缓存
                                CleanUtil.tryCleanMarsFiles();
                            }
                            return true;
                        }
                    }
                    // 不是目录或目录为空，删除
                    deleteFileOrDir(dirFile);
                }
            } catch (Exception e) {
                LogUtil.error(TAG, "checkDirExist.e:" + e.getMessage());
                return false;
            }
        }
        return false;
    }

    static void downloadAndUnzip(String zipUrl, String bizType, String dstPath, long timeStamp, String md5, DownloadZipCallback callback) {
        try {
            synchronized (sLockToken) {
                {
                    ArrayList<DownloadZipCallback> arr = sLoadingMap.get(zipUrl);
                    if (arr != null && arr.size() > 0) {
                        // 相同的下载地址正在下载，不再重复下载
                        LogUtil.debug(TAG, "duplicate loading,skip," + zipUrl);
                        arr.add(callback);
                        return;
                    }
                }
                String tmpPath = dstPath + "_tmp";
                File tmpFile = new File(tmpPath);
                if (tmpFile.exists()) {
                    // 临时目录存在，删除旧的临时目录
                    if (!deleteFileOrDir(tmpFile)) {
                        LogUtil.error(TAG, "remove old tmpDir fail:" + dstPath);
                        dispatchDownloadCallbackLocked(zipUrl, dstPath, "remove old tmpDir fail");
                        return;
                    }
                }
                LogUtil.debug(TAG, "start download");
                {
                    ArrayList<DownloadZipCallback> arr = new ArrayList<>();
                    arr.add(callback);
                    sLoadingMap.put(zipUrl, arr);
                }
                DownloadUtil.downloadFile(zipUrl, bizType, md5, new DownloadUtil.DownloadFileCallback() {
                    @Override
                    public void onSuccess(InputStream is, String filePath) {
                        synchronized (sLockToken) {
                            String dstDirPath = unzipSync(dstPath, tmpPath, filePath);
                            if (TextUtils.isEmpty(dstDirPath)) {
                                LogUtil.error(TAG, "unzip failed");
                                dispatchDownloadCallbackLocked(zipUrl, null, "unzip failed");
                            } else {
                                updateTimeStampLocked(new File(dstDirPath), timeStamp);
                                dispatchDownloadCallbackLocked(zipUrl, dstDirPath, null);
                            }
                        }
                    }

                    @Override
                    public void onError(String errMsg) {
                        synchronized (sLockToken) {
                            dispatchDownloadCallbackLocked(zipUrl, null, errMsg);
                        }
                    }
                });
            }
        } catch (Exception e) {
            LogUtil.error(TAG, "downloadAndUnzip.e:" + e.getMessage());
        }
    }

    /**
     * 删除超期没有访问的资源。如果未超期，不处理
     * @param dirFile 尝试删除的目录
     * @param expiredInterval 过期间隔
     * @return 是否有异常
     */
    static String deleteDirIfExpired(File dirFile, long expiredInterval) {
        try {
            synchronized (sLockToken) {
                if (System.currentTimeMillis() - getTimeStampLocked(dirFile) >= expiredInterval) {
                    LogUtil.debug(TAG, "delete," + dirFile.getPath());
                    if (!deleteFileOrDir(dirFile)) {
                        return "deleteFileOrDir fail";
                    }
                } else {
                    LogUtil.debug(TAG, "skip," + dirFile.getPath());
                }
            }
            return null;
        } catch (Exception e) {
            return e.getMessage();
        }
    }

    static String getMarsDir() {
        if (sMarsDir == null) {
            sMarsDir = ContextUtil.getApplicationContext().getFilesDir().getAbsolutePath()
                    + File.separator + "marsnative"
                    + File.separator;
        }
        return sMarsDir;
    }

    static boolean deleteFileOrDir(File file) {
        try {
            if (file == null || !file.exists()) {
                return false;
            }
            boolean success = true;
            if (file.isDirectory()) {
                File[] files = file.listFiles();
                if (files != null) {
                    for (File f : files) {
                        if (!deleteFileOrDir(f)) {
                            success = false;
                        }
                    }
                }
            }
            if (!file.delete()) {
                LogUtil.error(TAG, "deleteFileOrDir,fail:" + file.getAbsolutePath());
                return false;
            }
            return success;
        } catch (Exception e) {
            LogUtil.error(TAG, "deleteFileOrDir..e:" + file.getAbsolutePath() + "," + e.getMessage());
            return false;
        }
    }

    private static void dispatchDownloadCallbackLocked(String key, String dstPath, String errMsg) {
        try {
            ArrayList<DownloadZipCallback> arr = sLoadingMap.get(key);
            if (arr != null) {
                for (int i = 0; i < arr.size(); i++) {
                    DownloadZipCallback callback = arr.get(i);
                    callback.onResult(dstPath, errMsg);
                }
            }
            sLoadingMap.remove(key);
            if (sLoadingMap.isEmpty()) {
                CleanUtil.tryCleanMarsFiles();
            }
        } catch (Exception e) {
            LogUtil.error(TAG, "dispatchDownloadCallbackLocked.e:" + e.getMessage());
        }
    }

    private static void updateTimeStampLocked(File dirFile, long timeStamp) {
        File file = getTimeStampFile(dirFile);
        if (file == null) {
            return;
        }

        try {
            if (file.exists()) {
                boolean result = file.setLastModified(timeStamp);
                if (!result) {
                    LogUtil.error(TAG, "updateTimeStampLocked fail");
                    MonitorUtil.monitorCleanEvent("record", "update", false, dirFile.getName(), "updateTimeStampLocked fail");
                    return;
                }
                LogUtil.debug(TAG, "updateLastVisitTimeFile," + file + "," + timeStamp);
            }
        } catch (Exception e) {
            LogUtil.error(TAG, "updateTimeStampLocked.e:" + e.getMessage());
        }
    }

    private static long getTimeStampLocked(File dirFile) {
        File file = getTimeStampFile(dirFile);
        if (file == null) {
            return System.currentTimeMillis();
        }

        try {
            if (file.exists()) {
                long lastModified = file.lastModified();
                if (lastModified == 0) { // 文件最近修改时间错误，不删除文件
                    LogUtil.error(TAG, "lastModified empty");
                    MonitorUtil.monitorCleanEvent("record", "read", false, dirFile.getName(), "lastModified empty");
                    return System.currentTimeMillis();
                }
                return lastModified;
            } else {
                LogUtil.error(TAG, "getFileLastVisitTime not exist," + dirFile.getPath());
                return System.currentTimeMillis();
            }
        } catch (Exception e) {
            LogUtil.error(TAG, "getTimeStampLocked.e:" + e.getMessage());
            return System.currentTimeMillis();
        }
    }

    private static File getTimeStampFile(File dirFile) {
        if (CleanUtil.disabled()) {
            return null;
        }
        try {
            File file = new File(dirFile, "lastVisitTime");
            if (!file.exists()) {
                boolean success = file.createNewFile();
                if (!success) {
                    MonitorUtil.monitorCleanEvent("record", "create", false, dirFile.getName(), "createNewFile fail");
                    return null;
                }
            }
            return file;
        } catch (Exception e) {
            LogUtil.error(TAG, "getTimeStampFile.e:" + e.getMessage());
            return null;
        }
    }

    /**
     * 解压zip
     * @param dstDirPath 目标目录
     * @param tmpDirPath 临时存储目录
     * @param zipPath zip路径
     * @return 解压后的目录，null表示返回失败
     */
    private static String unzipSync(String dstDirPath, String tmpDirPath, String zipPath) {
        LogUtil.debug(TAG, "unzipSync " + dstDirPath + "," + tmpDirPath + "," + zipPath);
        if (TextUtils.isEmpty(dstDirPath) || TextUtils.isEmpty(tmpDirPath) || TextUtils.isEmpty(zipPath)) {
            return null;
        }

        final File dstDir = new File(dstDirPath);
        final File tmpDir = new File(tmpDirPath);
        if (!tmpDir.mkdirs()) {
            LogUtil.error(TAG, "unzipAndCopyFile:tmpDir mkdirs fail");
            return null;
        }

        boolean ret = false;
        FileInputStream zipFileIn = null;
        try {
            File zipFile = new File(zipPath);
            zipFileIn = new FileInputStream(zipFile);
            ret = ZipUtil.unZip(zipFileIn, tmpDir.getAbsolutePath() + File.separator);
            LogUtil.debug(TAG, "unzip result:" + ret);
        } catch (Exception e) {
            LogUtil.error(TAG, "unzip..e:" + e.getMessage());
            return null;
        } finally {
            IOUtil.closeStream(zipFileIn);
        }

        if (ret) {
            if (!dstDir.exists()) {
                ret = tmpDir.renameTo(dstDir);
                LogUtil.debug(TAG, "unzip rename result:" + ret);
            }
        }
        if (ret) {
            return dstDirPath;
        } else {
            return null;
        }
    }

    public interface DownloadZipCallback {
        void onResult(String dstPath, String errMsg);
    }

}
