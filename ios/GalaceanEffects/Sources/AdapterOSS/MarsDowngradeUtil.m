//
//  MarsDowngradeUtil.m
//
//  Created by Zongming Liu on 2022/4/26.
//  Copyright © 2022 Alipay. All rights reserved.
//

#import "MarsDowngradeUtil.h"
#import "MarsConfigUtil.h"
#import <UIKit/UIKit.h>
#import "CommonUtil.h"
#ifndef PLAYGROUND

#endif

static NSString* BIZ_ID = @"MarsNative";

@interface MarsDowngradeResult()

@property (nonatomic) BOOL downgrade;

@property (nonatomic, strong) NSString* reason;

@end

@implementation MarsDowngradeResult

- (instancetype)init {
    if (self = [super init]) {
        _downgrade = FALSE;
        _reason = @"";
    }
    return self;
}

- (BOOL) getDowngrade {
    return _downgrade;
}

- (NSString*) getReason {
    return _reason;
}

@end

static int s_device_level = -1;

@implementation MarsDowngradeUtil

+ (MarsDowngradeResult*) getDowngradeResult:(NSString*)resId {
    MarsDowngradeResult* ret = [[MarsDowngradeResult alloc] init];
    s_device_level = DEVICE_LEVEL_HIGH;
    return ret;
}

+ (void) writeResourceIdBegin:(NSString*)resId ThreadName:(NSString*)threadName {
}

+ (void) writeResourceIdFinish:(NSString*)resId ThreadName:(NSString*)threadName {
}

+ (int) getDeviceLevel {
    if (s_device_level == -1) {
        [MarsDowngradeUtil getDowngradeResult:@"test"];
    }
    return s_device_level;
}

@end
