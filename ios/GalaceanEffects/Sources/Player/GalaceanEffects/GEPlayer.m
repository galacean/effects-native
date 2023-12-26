//
//  GEPlayer.m
//  MarsNative
//
//  Created by changxing on 2023/11/16.
//  Copyright © 2023 Alipay. All rights reserved.
//

#import "GEPlayer.h"
#import "GEPlayerImpl.h"

@implementation GEPlayerParams
@end

@interface GEPlayer ()

@property (nonatomic, strong) GEPlayerImpl* impl;

@end

@implementation GEPlayer

- (instancetype)initWithParams:(GEPlayerParams *)params {
    if (self = [super init]) {
        self.impl = [[GEPlayerImpl alloc] initWithParams:params];
    }
    return self;
}

- (void)loadScene:(void(^)(bool success, NSString *errorMsg))callback {
    if (self.impl) {
        [self.impl loadScene:callback];
        UIView* view = [self.impl getView];
        if (view) {
            [self addSubview:view];
            [view setTranslatesAutoresizingMaskIntoConstraints:NO];
            [self addConstraint:[NSLayoutConstraint constraintWithItem:view
                                                             attribute:NSLayoutAttributeWidth
                                                             relatedBy:NSLayoutRelationEqual
                                                                toItem:self
                                                             attribute:NSLayoutAttributeWidth
                                                            multiplier:1.0
                                                              constant:0.0]];
            [self addConstraint:[NSLayoutConstraint constraintWithItem:view
                                                             attribute:NSLayoutAttributeHeight
                                                             relatedBy:NSLayoutRelationEqual
                                                                toItem:self
                                                             attribute:NSLayoutAttributeHeight
                                                            multiplier:1.0
                                                              constant:0.0]];
        }
    }
}

- (void)playWithRepeatCount:(int)repeatCount
                   Callback:(void(^ _Nullable)(bool success, NSString *errorMsg))callback {
    if (self.impl) {
        [self.impl playWithRepeatCount:repeatCount Callback:callback];
    }
}

- (void)playWithFromFrame:(int)fromFrame
                  ToFrame:(int)toFrame
              RepeatCount:(int)repeatCount
                 Callback:(void(^ _Nullable)(bool success, NSString *errorMsg))callback {
    if (self.impl) {
        [self.impl playWithFromFrame:fromFrame ToFrame:toFrame RepeatCount:repeatCount Callback:callback];
    }
}

- (void)pause {
    if (self.impl) {
        [self.impl pause];
    }
}

- (void)resume {
    if (self.impl) {
        [self.impl resume];
    }
}

- (void)stop {
    if (self.impl) {
        [self.impl stop];
    }
}

- (void)destroy {
    if (self.impl) {
        [self.impl destroy];
    }
}

- (float)getAspect {
    if (self.impl) {
        return [self.impl getAspect];
    }
    return -1;
}

- (int)getFrameCount {
    if (self.impl) {
        return [self.impl getFrameCount];
    }
    return -1;
}

@end
