package com.alipay.mobile.mars.adapter;

import java.security.MessageDigest;

public class MD5Util {

    private static char hexDigests[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

    public static String getMD5String(String content) {
        try {
            byte[] in = content.getBytes();
            MessageDigest messageDigest = MessageDigest.getInstance("md5");
            messageDigest.update(in);

            byte[] md = messageDigest.digest();
            int j = md.length;
            char[] str = new char[j*2];
            int k = 0;
            for (int i = 0; i < j; i++) {
                byte b = md[i];
                str[k++] = hexDigests[b >>> 4 & 0xf];
                str[k++] = hexDigests[b & 0xf];
            }
            return new String(str);
        } catch (Throwable e) {
            e.printStackTrace();
            return content;
        }
    }

}
