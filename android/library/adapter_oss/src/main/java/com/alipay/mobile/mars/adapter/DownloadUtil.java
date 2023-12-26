package com.alipay.mobile.mars.adapter;

import android.support.annotation.NonNull;
import android.text.TextUtils;

import com.alipay.mobile.mars.util.IOUtil;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.InputStream;

import okhttp3.OkHttpClient;

public class DownloadUtil {

    private final static String TAG = "MultiMediaServiceAdapter";

    /**
     * 异步下载文件
     * @param url djangoId或网络地址
     * @param bizType 业务埋点
     * @param md5 md5，null表示不进行校验
     * @param callback 回调
     */
    public static void downloadFile(final String url, String bizType, String md5, final DownloadFileCallback callback) {
        APFileReq req = new APFileReq();
        req.setCloudId(url);
        if (!TextUtils.isEmpty(md5)) {
            // 如果存在md5，设置校验md5
            req.setMd5(md5);
        }

        downLoad(req, new APFileDownCallback() {
            @Override
            public void onDownloadStart() {
                LogUtil.debug(TAG, "onDownloadStart " + url);
            }

            @Override
            public void onDownloadFinished(APFileDownloadRsp apFileDownloadRsp) {
                LogUtil.debug(TAG, "onDownloadFinished " + url + "," + apFileDownloadRsp);
                String savePath = apFileDownloadRsp.getFileReq().getSavePath();
                FileInputStream is = null;

                try {
                    is = new FileInputStream(savePath);
                    callback.onSuccess(is, savePath);
                } catch (Exception e) {
                    LogUtil.error(TAG, "onDownloadFinished..e:" + e.getMessage());
                    callback.onError("downloadFile..e:" + e.getMessage());
                } finally {
                    IOUtil.closeStream(is);
                }
            }

            @Override
            public void onDownloadError(APFileDownloadRsp apFileDownloadRsp) {
                LogUtil.error(TAG, "onDownloadError " + url + "," + apFileDownloadRsp);
                callback.onError("downloadFile..e:" + apFileDownloadRsp.getMsg());
            }
        }, TextUtils.isEmpty(bizType) ? "MarsPlayer" : bizType);
    }

    /**
     * 获得多媒体缓存路径
     * @param url djangoId或网络地址
     * @return 缓存路径，null表示没有找到
     */
    public static String getDownloadCachePath(final String url) {
        String cachePath = getCachePath(url);
        File cacheFile = new File(cachePath);
        return cacheFile.exists() ? cachePath : null;
    }

    public interface DownloadFileCallback {
        /**
         * 下载文件成功回调
         * @param is InputStream在回调执行结束后自动释放
         * @param filePath 文件路径
         */
        void onSuccess(InputStream is, String filePath);

        void onError(String errMsg);
    }

    private static void downLoad(final APFileReq req, final APFileDownCallback callback, String bizId) {
        TaskScheduleUtil.postToNormalThread(new Runnable() {
            @Override
            public void run() {
                APFileDownloadRsp rsp = new APFileDownloadRsp(req);
                boolean success = false;
                FileOutputStream fileOutputStream = null;
                callback.onDownloadStart();
                try {
                    do {
                        String cachePath = getCachePath(req.getUrl());
                        req.setCachePath(cachePath);
                        File cacheFile = new File(cachePath);
                        if (cacheFile.exists()) {
                            success = true;
                            break;
                        }
                        File parentDir = cacheFile.getParentFile();
                        if (!parentDir.exists()) {
                            parentDir.mkdirs();
                        }
                        OkHttpClient client = new OkHttpClient();
                        okhttp3.Request request = new okhttp3.Request.Builder().url(req.getUrl()).build();
                        okhttp3.Response response = client.newCall(request).execute();//发送请求
                        if (response.body() == null) {
                            rsp.setMsg("response.body() is null");
                            break;
                        }
                        byte[] data = response.body().bytes();
                        cacheFile.createNewFile();
                        fileOutputStream = new FileOutputStream(cacheFile);
                        fileOutputStream.write(data, 0, data.length);
                        fileOutputStream.flush();

                        success = true;
                    } while (false);
                } catch (Exception e) {
                    LogUtil.error(TAG, "download.e:" + e.getMessage());
                    rsp.setMsg(e.getMessage());
                    success = false;
                } finally {
                    try {
                        if (fileOutputStream != null) {
                            fileOutputStream.close();
                        }
                    } catch (Exception e) {
                        e.printStackTrace();
                    }
                }
                if (success) {
                    callback.onDownloadFinished(rsp);
                } else {
                    callback.onDownloadError(rsp);
                }
            }
        });
    }

    private static String getCachePath(String url) {
        String fileName = MD5Util.getMD5String(url) + ".tmp";
        return getCacheDirPath() + fileName;
    }

    private static String sCacheDir = null;
    private static synchronized String getCacheDirPath() {
        if (sCacheDir == null) {
            sCacheDir= ContextUtil.getApplicationContext().getFilesDir().getAbsolutePath()
                    + File.separator + "mars-files"
                    + File.separator;
        }
        return sCacheDir;
    }

    public static class APFileReq {

        private String url;

        private String md5;

        private String cachePath;

        public void setCloudId(String cloudId) {
            url = cloudId;
        }

        public String getUrl() {
            return url;
        }

        public void setMd5(String md5) {
            this.md5 = md5;
        }

        public void setCachePath(String cachePath) {
            this.cachePath = cachePath;
        }

        public String getSavePath() {
            return cachePath;
        }

    }

    private static class APFileDownloadRsp {

        private final APFileReq req;
        private String msg = "";

        public APFileDownloadRsp(APFileReq req) {
            this.req = req;
        }

        public void setMsg(String m) {
            msg = m;
        }

        public String getMsg() {
            return msg;
        }

        public APFileReq getFileReq() {
            return req;
        }

        @NonNull
        @Override
        public String toString() {
            return "APFileDownloadRsp@" + hashCode() + "_" + msg;
        }

    }

    private interface APFileDownCallback {

        void onDownloadStart();

        void onDownloadFinished(APFileDownloadRsp apFileDownloadRsp);

        void onDownloadError(APFileDownloadRsp apFileDownloadRsp);
        
    }

}



