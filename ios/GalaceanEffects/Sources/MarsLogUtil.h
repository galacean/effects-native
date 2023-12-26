//
//  MarsLogUtil.h
//  Mars
//
//  Created by changxing on 2022/8/24.
//  Copyright © 2022 Alipay. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol IMarsLogger <NSObject>

- (void)debug:(NSString*) tag Msg:(NSString*) msg;

- (void)error:(NSString*) tag Msg:(NSString*) msg;

@end

@interface MarsLogUtil : NSObject

+ (void)setLogger:(id<IMarsLogger>)logger;

+ (id<IMarsLogger>)getLogger;

+ (void)debug:(NSString*) tag Msg:(NSString*) msg;

+ (void)error:(NSString*) tag Msg:(NSString*) msg;

@end

NS_ASSUME_NONNULL_END
