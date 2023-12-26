package com.alipay.mobile.mars;

/**
 * 仅供线下测试，相关代码不要带到线上去
 */
public class MemoryDebugger {

//    private static final boolean ENABLE = true;

//    private static final Map<Integer, String> sPlayerMap = new HashMap<>();

    public static void onCreatePlayer(String sourceId, int nativeId) {
//        if (!ENABLE) {
//            return;
//        }
//        if (nativeId == -1) {
//            return;
//        }
//        synchronized (MemoryDebugger.class) {
//            sPlayerMap.put(nativeId, sourceId);
//            monitorEvent("create");
//        }
    }

    public static void onDestroyPlayer(int nativeId) {
//        if (!ENABLE) {
//            return;
//        }
//        if (nativeId == -1) {
//            return;
//        }
//        synchronized (MemoryDebugger.class) {
//            sPlayerMap.remove(nativeId);
//            monitorEvent("destroy");
//        }
    }

//    private static void monitorEvent(String scene) {
//        AntEvent.Builder eventBuilder = new AntEvent.Builder();
//        eventBuilder.setEventID("1010790");
//        eventBuilder.setBizType("middle");
//        eventBuilder.setLoggerLevel(2);
//
//        eventBuilder.addExtParam("scene", scene);
//        eventBuilder.addExtParam("count", sPlayerMap.size() + "");
//        StringBuilder str = new StringBuilder();
//        for (Integer key : sPlayerMap.keySet()) {
//            str.append(key).append(":").append(sPlayerMap.get(key)).append(";");
//        }
//        eventBuilder.addExtParam("data", str.toString());
//        eventBuilder.build().send();
//    }

}
