//
//  GEPlayerImpl.h
//  MarsNative
//
//  Created by changxing on 2023/11/16.
//  Copyright © 2023 Alipay. All rights reserved.
//

#import <UIKit/UIKit.h>

NS_ASSUME_NONNULL_BEGIN

@class GEPlayerParams;

@interface GEPlayerImpl : NSObject

- (instancetype)initWithParams:(GEPlayerParams *)params;

- (UIView *)getView;

- (void)loadScene:(void(^)(bool success, NSString *errorMsg))callback;

- (void)playWithRepeatCount:(int)repeatCount
                   Callback:(void(^ _Nullable)(bool success, NSString *errorMsg))callback;

- (void)playWithFromFrame:(int)fromFrame
                  ToFrame:(int)toFrame
              RepeatCount:(int)repeatCount
                 Callback:(void(^ _Nullable)(bool success, NSString *errorMsg))callback;

- (void)pause;

- (void)resume;

- (void)stop;

- (void)destroy;

- (float)getAspect;

- (int)getFrameCount;

@end

NS_ASSUME_NONNULL_END
