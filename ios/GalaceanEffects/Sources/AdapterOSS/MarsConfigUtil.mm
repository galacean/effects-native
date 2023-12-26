//
//  MarsConfigCenter.m
//  MarsNative
//
//  Created by Zongming Liu on 2022/5/8.
//  Copyright © 2022 Alipay. All rights reserved.
//

#import "MarsConfigUtil.h"
#import "MarsAPLog.h"
#import "CommonUtil.h"
#import <UIKit/UIKit.h>

#ifndef PLAYGROUND
#endif

@interface MarsConfigUtil()

#ifndef PLAYGROUND
//@property(nonatomic, strong) id<APConfigService> apConfigService;
#endif

@end

@implementation MarsConfigUtil

+ (instancetype) shareInstance
{
    static MarsConfigUtil *sharedInstance;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedInstance = [[MarsConfigUtil alloc] initPrivate];
    });
    
    return sharedInstance;
}

- (instancetype) initPrivate {
    if (self = [super init]) {
#ifndef PLAYGROUND
//        _apConfigService = [DTContextGet() findServiceByName:@"APConfigService"];
#endif
    }
    return self;
}


- (BOOL)forceDowngradeCompressed {
    return [self boolValueForKey:@"mn_forceDowngradeCompressed"];
}

- (BOOL)forceDowngradeMarsNative {
    return [self boolValueForKey:@"mn_forceDowngradeMN"];
}

- (BOOL)forceDowngradeByResId:(NSString*) resId {
    if (resId) {
#ifndef PLAYGROUND
        resId = [CommonUtil getMD5String:resId];
#endif
    }
    return [self forceDowngradeByResScene:resId];
}

- (BOOL)forceDowngradeByResScene:(NSString*)scene {
    if (!scene.length) {
        return false;
    }
    return [self boolValueForKey:[NSString stringWithFormat:@"mn_dg_%@", scene]];
}

- (BOOL)forceDowngradeScreenDpi:(NSString*)resId {
    if (!resId || resId.length == 0) {
        return NO;
    }

    return [self compareValueForKey:@"mn_forceDowngradeScreenDpi" compareString:resId];
}

- (int)getRenderLevelWithDefaultVal:(int)defaultVal {
    return [self intValueForKey:@"mn_getRenderLevel" defaultVal:defaultVal];
}

// 判断当前资源是否使用AVSimpleBufferLayer
- (BOOL)useAVSimpleBufferLayer:(NSString*)resId {
    @try {
        if ([UIDevice currentDevice].systemVersion.doubleValue < 15.0) { // ios 15以下不支持
            return NO;
        }
        NSString* content = [self configForKey:@"mn_useAVSimpleBufferLayer"];
        if (!content.length) { // 开关为空，不用
            return NO;
        }
        NSData* jsonData = [content dataUsingEncoding:NSUTF8StringEncoding];
        NSError* error = nil;
        NSDictionary* dictionary = [NSJSONSerialization JSONObjectWithData:jsonData options:kNilOptions error:&error];
        if (error) { // 开关值转json失败，不用
            MNLogInfo(@"useAVSimpleBufferLayer err:%@", error.description);
            return NO;
        }
        if (!dictionary) { // 开关转json为空，不用
            MNLogInfo(@"useAVSimpleBufferLayer empty dict");
            return NO;
        }
        NSString* whiteList = [dictionary objectForKey:@"whiteList"];
        if ([whiteList isEqualToString:@"all"]) { // 白名单为all，判断是否在黑名单
            NSString* blackList = [dictionary objectForKey:@"blackList"];
            if ([blackList containsString:resId]) { // 在黑名单，不用
                return NO;
            }
            // 不在黑名单，用
            MNLogInfo(@"useAVSimpleBufferLayer true");
            return YES;
        } else { // 白名单不为all，判断是否包含当前resId
            return [whiteList containsString:resId];
        }
    } @catch (NSException *exception) {
        MNLogInfo(@"useAVSimpleBufferLayer.e:%@", exception.description);
    } @finally {
    }
}

// 视频是否硬解码
- (BOOL)enableVideoHardDecoder:(NSString*)resId {
    return NO;
}

- (BOOL)usePThread {
    return [self boolValueForKey:@"mn_usePThread" defaultVal:YES];
}

- (NSString *)configForKey:(NSString *)key
{
//#ifndef PLAYGROUND
//    NSString *config = [_apConfigService stringValueForKey:key];
//    return config;
//#else
    return @"";
//#endif
}

- (int)intValueForKey:(NSString*)key defaultVal:(int)defaultVal {
    int ret = defaultVal;
    NSString* value = [self configForKey:key];
    if (value.length && [value respondsToSelector:@selector(intValue)]) {
        ret = [value intValue];
    }
    return ret;
}

#pragma mark - Private

- (BOOL)boolValueForKey:(NSString *)key
{
    return [self boolValueForKey:key defaultVal:NO];
}

- (BOOL)boolValueForKey:(NSString *)key defaultVal:(BOOL)defaultVal
{
    BOOL ret = defaultVal;
    id value = [self configForKey:key];
    if (value && [value respondsToSelector:@selector(boolValue)]) {
        ret = [value boolValue];
    }
    return ret;
}
    
- (BOOL)compareValueForKey:(NSString *)key compareString:(NSString *)compareString {
    id value = [self configForKey:key];
    if (value && [value isKindOfClass:[NSString class]]) {
        if ([value containsString:@"all"]) {
            return YES;
        } else {
            return [value containsString:compareString];
        }
    } else {
        return NO;
    }
}

@end
