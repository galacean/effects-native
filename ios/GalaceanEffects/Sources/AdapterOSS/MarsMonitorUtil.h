//
//  MarsNativeMonitor.h
//  MarsNative
//
//  Created by Zongming Liu on 2022/5/9.
//  Copyright © 2022 Alipay. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

#define INIT_ERROR     @"init_error"
#define RUNTIME_ERROR   @"runtime_error"

@interface MarsMonitorUtil : NSObject

+ (void)monitorMarsNativeStart:(NSString *)sourceId Source:(NSString*)source IsMarsPlayer:(bool)isMarsPlayer;

+ (void)monitorMarsNativeError:(NSString *)sourceId errorType:(NSString *)code errorMsg:(NSString *)msg;

+ (void)monitorMarsNativeStatistics:(NSString *)sourceId supportCompressedTexture:(bool)support glesVersion:(int)glesVersion;

+ (void)monitorMarsNativeClean:(NSString*)type Opt:(NSString*)operation Succ:(BOOL)success ErrFile:(nullable NSString*)errFile ErrMsg:(nullable NSString*)errMsg;

@end

NS_ASSUME_NONNULL_END
