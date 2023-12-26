//
//  MarsDowngradeUtil.h
//
//  Created by Zongming Liu on 2022/4/26.
//  Copyright © 2022 Alipay. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface MarsDowngradeResult : NSObject

- (BOOL) getDowngrade;

- (NSString*) getReason;

@end

@interface MarsDowngradeUtil : NSObject

+ (MarsDowngradeResult*) getDowngradeResult:(NSString*)resId;

+ (void) writeResourceIdBegin:(NSString*)resId ThreadName:(NSString*)threadName;

+ (void) writeResourceIdFinish:(NSString*)resId ThreadName:(NSString*)threadName;

+ (int) getDeviceLevel;

@end

NS_ASSUME_NONNULL_END
