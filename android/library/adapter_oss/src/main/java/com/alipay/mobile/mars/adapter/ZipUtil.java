package com.alipay.mobile.mars.adapter;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class ZipUtil {
    
    private static boolean isSafeEntryName(String path) {
        return !path.contains("..") && !path.contains("%");
    }

    public static boolean unZip(FileInputStream zipFileIn, String targetDir) {
        try {
            if (targetDir.contains("../")) {
                LogUtil.error("ZipHelper", "invalid targetDir:" + targetDir);
                return false;
            }
            ZipInputStream zipInputStream = new ZipInputStream(zipFileIn);
            ZipEntry zipEntry = zipInputStream.getNextEntry();
            byte[] buffer = new byte[1024 * 1024];
            int count = 0;
            while (zipEntry != null) {
                if (!zipEntry.isDirectory()) {
                    String fileName = zipEntry.getName();
                    if (!isSafeEntryName(fileName)) {
                        LogUtil.error("ZipHelper", "invalid fileName:" + fileName);
                        return false;
                    }
                    String filePath = targetDir + "/" + fileName;
                    if (filePath.contains("../")) {
                        LogUtil.error("ZipHelper", "invalid filePath:" + fileName);
                        return false;
                    }
                    File file = new File(filePath);
                    File parent = file.getParentFile();
                    if (!parent.exists()) {
                        parent.mkdirs();
                    }
                    if (file.exists()) {
                        file.delete();
                    }
                    file.createNewFile();
                    FileOutputStream fileOutputStream = new FileOutputStream(file);
                    while ((count = zipInputStream.read(buffer)) > 0) {
                        fileOutputStream.write(buffer, 0, count);
                    }
                    fileOutputStream.close();
                }
                zipEntry = zipInputStream.getNextEntry();
            }
            return true;
        } catch (Exception e) {
            e.printStackTrace();
            return false;
        }
    }

}