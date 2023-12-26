//
//  MarsLogUtil.m
//  Mars
//
//  Created by changxing on 2022/8/24.
//  Copyright © 2022 Alipay. All rights reserved.
//

#import "MarsLogUtil.h"
#include "util/log_util.hpp"

static id<IMarsLogger> s_logger = nil;

@implementation MarsLogUtil

+ (void)setLogger:(id<IMarsLogger>)logger {
    s_logger = logger;
}

+ (id<IMarsLogger>)getLogger {
    return s_logger;
}

+ (void)debug:(NSString*) tag Msg:(NSString*) msg {
    if (s_logger) {
        [s_logger debug:tag Msg:msg];
    }
}

+ (void)error:(NSString*) tag Msg:(NSString*) msg {
    if (s_logger) {
        [s_logger error:tag Msg:msg];
    }
}

@end

namespace mn {

void LogUtil::platform_log(int level, const char* msg) {
    if (level == 4) {
        [MarsLogUtil error:@"[MN::]" Msg:[NSString stringWithUTF8String:msg]];
    } else {
        [MarsLogUtil debug:@"[MN::]" Msg:[NSString stringWithUTF8String:msg]];
    }
}

}
