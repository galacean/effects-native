//
//  MarsNativeUnzipProtocol.m
//  MarsNative
//
//  Created by changxing on 2023/11/15.
//  Copyright © 2023 Alipay. All rights reserved.
//
#import "GEUnzipProtocol.h"

static GEUnzipManager* sInstance = nil;

@implementation GEUnzipManager

+(GEUnzipManager*) shared {
    if (sInstance == nil) {
        sInstance = [[GEUnzipManager alloc] init];
    }
    return sInstance;
}

@end
