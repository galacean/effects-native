//
//  MarsConfigCenter.h
//  MarsNative
//
//  Created by Zongming Liu on 2022/5/8.
//  Copyright © 2022 Alipay. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface MarsConfigUtil : NSObject

+ (instancetype)shareInstance;

// 是否强制降级压缩纹理
- (BOOL)forceDowngradeCompressed;

// 是否强制降级MarsNative
- (BOOL)forceDowngradeMarsNative;

// 是否根据resId降级
- (BOOL)forceDowngradeByResId:(NSString*)resId;

// 是否根据场景降级
- (BOOL)forceDowngradeByResScene:(NSString*)scene;

// 是否根据resId降级分辨率
- (BOOL)forceDowngradeScreenDpi:(NSString*)resId;

// 1: low, 2: medium, 4: high
- (int)getRenderLevelWithDefaultVal:(int)defaultVal;

// 是否使用AVSimpleBufferLayer进行渲染
- (BOOL)useAVSimpleBufferLayer:(NSString*)resId;

// 视频是否硬解码
- (BOOL)enableVideoHardDecoder:(NSString*)resId;

/// 是否使用c++的线程tick，默认开启。80版本测试，90版本下线
- (BOOL)usePThread;

- (NSString *)configForKey:(NSString *)key;

- (int)intValueForKey:(NSString*)key defaultVal:(int)defaultVal;

@end

NS_ASSUME_NONNULL_END
