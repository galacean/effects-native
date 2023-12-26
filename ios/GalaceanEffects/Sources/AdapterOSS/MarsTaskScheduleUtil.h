//
//  TaskScheduleAdapter.h
//  MarsNative
//
//  Created by changxing on 2023/2/8.
//  Copyright © 2023 Alipay. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface MarsTaskScheduleUtil : NSObject

+ (void)postToUIThread:(dispatch_block_t)r Delayd:(double)delayMs;

+ (void)postToNormalThread:(dispatch_block_t)r;

@end

NS_ASSUME_NONNULL_END
