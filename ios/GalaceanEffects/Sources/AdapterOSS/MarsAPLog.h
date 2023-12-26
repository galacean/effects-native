//
//  MarsAPLog.h
//  MarsNative
//
//  Created by changxing on 2022/5/11.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef MarsAPLog_h
#define MarsAPLog_h

#import <Foundation/Foundation.h>
#import "MarsLogUtil.h"

void MNLogInfo(NSString* format, ...);

@interface MarsAPLogger : NSObject <IMarsLogger>

- (void)debug:(NSString*) tag Msg:(NSString*) msg;

- (void)error:(NSString*) tag Msg:(NSString*) msg;

+ (void)Create;

@end

#endif /* MarsAPLog_h */
