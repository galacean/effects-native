//
//  MarsEventEmitter.h
//  MarsNative
//
//  Created by changxing on 2022/5/13.
//  Copyright © 2022 Alipay. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@protocol MarsEventDelegate <NSObject>

- (void)onEventType:(int)type Msg:(NSString*)msg;

@end

@interface MarsEventEmitter : NSObject

+ (void)onEvent:(int)player_id Type:(int)type Msg:(NSString*)msg;

+ (void)registerPlayerId:(int)player_id Listener:(id<MarsEventDelegate>)eventDelegate;

+ (void)unregisterPlayerId:(int)player_id;

@end

NS_ASSUME_NONNULL_END
