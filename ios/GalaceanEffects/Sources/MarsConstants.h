//
//  MarsConstants.h
//  MarsNative
//
//  Created by changxing on 2023/2/8.
//  Copyright © 2023 Alipay. All rights reserved.
//

#ifndef MarsConstants_h
#define MarsConstants_h

#define DEVICE_LEVEL_HIGH (4)

#define DEVICE_LEVEL_MEDIUM (2)

#define DEVICE_LEVEL_LOW (1)

#define ERROR_LOAD_BITMAP_FAIL (1001)

#define ERROR_LOAD_ZIP_FAIL (1002)

#define ERROR_CREATE_MARS_DATA_FAIL (1003)

#define ERROR_CREATE_PLAYER_FAIL (1004)

#define ERROR_DOWNGRADE (1005)

/// e.g. assets://MarsNative.bundle/marsfiles
#define RES_PATH_ASSETS_PREFIX (@"assets://")

// 支付宝不能支持访问本地磁盘，打开要拉安全同学评估 from 夙兮
//#define RES_PATH_FILE_PREFIX (@"file://")

#define NATIVE_EVENT_PAUSE (1)

#define NATIVE_EVENT_RESUME (2)

#define PLATFORM_EVENT_STATISTICS (1)
#define PLATFORM_EVENT_START (2)
#define PLATFORM_EVENT_THREAD_START (3)
#define PLATFORM_EVENT_THREAD_END (4)
#define PLATFORM_EVENT_ANIMATION_END (5)
#define PLATFORM_EVENT_INTERACT_MESSAGE_BEGIN (6)
#define PLATFORM_EVENT_INTERACT_MESSAGE_END (7)
#define PLATFORM_EVENT_EGL_INIT_ERROR (100)
#define PLATFORM_EVENT_RUNTIME_ERROR (101)

#endif /* MarsConstants_h */
