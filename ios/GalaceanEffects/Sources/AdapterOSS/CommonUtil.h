//
//  CommonUtil.h
//  MarsNative
//
//  Created by changxing on 2022/10/11.
//  Copyright © 2022 Alipay. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

#define DEVICE_LEVEL_HIGH (4)

#define DEVICE_LEVEL_MEDIUM (2)

#define DEVICE_LEVEL_LOW (1)

@interface CommonUtil : NSObject

+ (int) parseDeviceLevelString:(NSString*) str;

+ (NSError*)makeError:(NSString*)domain Code:(int)code Msg:(NSString*)msg;

+ (NSString*)getMD5String:(NSString*)str;

/// 格式化时间戳
/// - Parameter timeInterval: 时间戳（精确到秒）
+ (NSString*)formatTimeStamp:(NSTimeInterval)timeInterval;

@end

NS_ASSUME_NONNULL_END
