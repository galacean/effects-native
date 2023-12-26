//
//  GEPlayerImpl.m
//  MarsNative
//
//  Created by changxing on 2023/11/16.
//  Copyright © 2023 Alipay. All rights reserved.
//

#import "GEPlayerImpl.h"
#import "GEPlayer.h"
#import "MarsNativeBuilder.h"
#import "MarsTaskScheduleUtil.h"
#import "MarsAPLog.h"

@interface GEPlayerImpl()

@property (nonatomic, strong) GEPlayerParams *params;

@property (nonatomic, strong) MarsNativePlayer *player;

@end

static NSString* TAG = @"GEPlayerImpl";

@implementation GEPlayerImpl

- (instancetype)initWithParams:(GEPlayerParams *)params {
    if (self = [super init]) {
        self.params = params;
        self.player = nil;
    }
    return self;
}

- (UIView*)getView {
    return self.player;
}

- (void)loadScene:(void (^)(bool, NSString * _Nonnull))callback {
    if (self.player) {
        [MarsTaskScheduleUtil postToUIThread:^{
            if (callback) {
                callback(false, @"loadScene duplicated");
            }
        } Delayd:0];
        return;
    }
    if (!self.params || !self.params.url.length) {
        [MarsTaskScheduleUtil postToUIThread:^{
            if (callback) {
                callback(false, @"url is null");
            }
        } Delayd:0];
        return;
    }
    @try {
        MarsNativeBuilder *builder = [[MarsNativeBuilder alloc] initWithSource:@"galacean" Url:self.params.url];
        builder.placeHolderBitmap = self.params.downgradeImage;
        builder.variables = self.params.variables;
        builder.variablesBitmap = self.params.variablesBitmap;
        builder.showPlaceHolderFirst = NO;
        NSError *err = nil;
        self.player = [builder build:&err];
        __weak GEPlayerImpl *weakThiz = self;
        [builder initPlayerWithOnShowPlaceHolder:^{
        } Complete:^(bool success, NSError * _Nullable error) {
            if (!callback) {
                MNLogInfo(@"%@ onInitResult without callback", TAG);
                return;
            }
            __strong GEPlayerImpl *thiz = weakThiz;
            if (!thiz || !thiz.player) { // destroy之后不再回调
                return;
            }
            if (success) {
                callback(true, @"");
            } else {
                callback(false, error.description);
            }
        }];
    } @catch (NSException *exception) {
        MNLogInfo(@"loadScene.e:%@", exception.description);
        [MarsTaskScheduleUtil postToUIThread:^{
            if (callback) {
                callback(false, exception.description);
            }
        } Delayd:0];
    }
}

- (void)playWithRepeatCount:(int)repeatCount
                   Callback:(void (^)(bool, NSString * _Nonnull))callback {
    if (self.player) {
        [self.player playWithRepeatCount:repeatCount Complete:^(bool success, NSError *error) {
            @try {
                if (callback) {
                    callback(success, error.description);
                }
            } @catch (NSException *exception) {
                MNLogInfo(@"%@ onPlayFinish.e:%@", TAG, exception.description);
            }
        }];
    }
}

- (void)playWithFromFrame:(int)fromFrame
                  ToFrame:(int)toFrame
              RepeatCount:(int)repeatCount
                 Callback:(void (^)(bool, NSString * _Nonnull))callback {
    if (self.player) {
        [self.player playWithStartFrame:fromFrame
                               EndFrame:toFrame
                            RepeatCount:repeatCount
                               Complete:^(bool success, NSError *error) {
            @try {
                if (callback) {
                    callback(success, error.description);
                }
            } @catch (NSException *exception) {
                MNLogInfo(@"%@ onPlayFinish.e:%@", TAG, exception.description);
            }
        }];
    }
}

- (void)pause {
    if (self.player) {
        [self.player pause];
    }
}

- (void)resume {
    if (self.player) {
        [self.player resume];
    }
}

- (void)stop {
    if (self.player) {
        [self.player stop];
    }
}

- (void)destroy {
    if (self.player) {
        [self.player destroy];
        self.player = nil;
    }
}

- (float)getAspect {
    if (self.player) {
        return [self.player getAspect];
    }
    return -1;
}

- (int)getFrameCount {
    if (self.player) {
        return (int) ([self.player getDuration] * 1000.0 / 33.0);
    }
    return -1;
}

@end
