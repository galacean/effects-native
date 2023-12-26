//
//  MarsNativeMonitor.m
//  MarsNative
//
//  Created by Zongming Liu on 2022/5/9.
//  Copyright © 2022 Alipay. All rights reserved.
//

#import "MarsMonitorUtil.h"
#import "MarsAPLog.h"

static NSString *bizType              = @"middle";
static NSString *startEventId         = @"1011311";
static NSString *errorEventId         = @"1020216";
static NSString *statisticsEventId         = @"1010928";
static NSString *cleanEventId         = @"102190";

@implementation MarsMonitorUtil

+ (void)monitorMarsNativeStart:(NSString *)sourceId Source:(NSString*)source IsMarsPlayer:(bool)isMarsPlayer {
    MNLogInfo(@"monitorMarsNativeStart %@ %@ %d", sourceId, source, isMarsPlayer);
}

+ (void)monitorMarsNativeError:(NSString *)sourceId errorType:(NSString *)type errorMsg:(NSString *)msg {
    MNLogInfo(@"monitorMarsNativeError %@ %@ %@", sourceId, type, msg);
}

+ (void)monitorMarsNativeStatistics:(NSString *)sourceId supportCompressedTexture:(bool)support glesVersion:(int)glesVersion {
    MNLogInfo(@"monitorMarsNativeStatistics %@ %d %d", sourceId, support, glesVersion);
}

+ (void)monitorMarsNativeClean:(NSString*)type Opt:(NSString*)operation Succ:(BOOL)success ErrFile:(nullable NSString*)errFile ErrMsg:(nullable NSString*)errMsg {
    MNLogInfo(@"monitorMarsNativeClean %@ %@ %d %@ %@", type, operation, success, errFile, errMsg);
}

@end
