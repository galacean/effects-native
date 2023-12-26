//
//  MarsEventEmitter.m
//  MarsNative
//
//  Created by changxing on 2022/5/13.
//  Copyright © 2022 Alipay. All rights reserved.
//

#import "MarsEventEmitter.h"

static NSMutableDictionary* sListeners;
static NSString* sLockToken = @"event_token";

@implementation MarsEventEmitter

+ (void)onEvent:(int)player_id Type:(int)type Msg:(NSString*)msg {
    @synchronized (sLockToken) {
        id<MarsEventDelegate> listener = [sListeners objectForKey:@(player_id)];
        if (listener) {
            [listener onEventType:type Msg:msg];
        }
    }
}

+ (void)registerPlayerId:(int)player_id Listener:(id<MarsEventDelegate>)eventDelegate {
    @synchronized (sLockToken) {
        if (!sListeners) {
            sListeners = [[NSMutableDictionary alloc] init];
        }
        [sListeners setObject:eventDelegate forKey:@(player_id)];
    }
}

+ (void)unregisterPlayerId:(int)player_id {
    @synchronized (sLockToken) {
        [sListeners removeObjectForKey:@(player_id)];
    }
}

@end
