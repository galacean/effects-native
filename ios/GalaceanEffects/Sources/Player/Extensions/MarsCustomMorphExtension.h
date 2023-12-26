//
//  MarsCustomMorphExtension.h
//  MarsNative
//
//  Created by changxing on 2023/9/14.
//  Copyright © 2023 Alipay. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <MarsNativeBuilder.h>

NS_ASSUME_NONNULL_BEGIN

@interface MarsCustomMorphExtension : NSObject <MarsNativeExtension>

- (instancetype)initWithParams:(NSArray<NSString*>*) params;

/**
 * 业务自行实现，获取动画相关参数
 */
- (void)onSceneDataParamsDuration:(float)duration;

/**
 * 业务自行实现，动态修改遮罩
 * time: 0 - 1
 */
- (NSArray*)generateMorphData:(NSString*)name Time:(float)time;

@end

NS_ASSUME_NONNULL_END
