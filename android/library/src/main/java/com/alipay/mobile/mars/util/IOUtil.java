package com.alipay.mobile.mars.util;

import android.util.Log;

import java.io.IOException;
import java.io.InputStream;

public class IOUtil {
    
    public static void closeStream(InputStream stream) {
        try {
            if (stream != null)
                stream.close();
        } catch (IOException e) {
            Log.e("IOUtil", "", e);
        }
    }
    
}
