//
//  TaskScheduleAdapter.m
//  MarsNative
//
//  Created by changxing on 2023/2/8.
//  Copyright © 2023 Alipay. All rights reserved.
//

#import "MarsTaskScheduleUtil.h"

@implementation MarsTaskScheduleUtil

+ (void)postToUIThread:(dispatch_block_t)r Delayd:(double)delayMs {
    dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(delayMs * NSEC_PER_MSEC)), dispatch_get_main_queue(), r);
}

+ (void)postToNormalThread:(dispatch_block_t)r {
    dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), r);
}

@end
