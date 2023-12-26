//
//  MarsPositionPathModifierExtension.h
//  MarsNative
//
//  Created by changxing on 2023/9/12.
//  Copyright © 2023 Alipay. All rights reserved.
//

#import <Foundation/Foundation.h>
#import <MarsNativeBuilder.h>

NS_ASSUME_NONNULL_BEGIN

@interface MarsPositionPathModifyResult : NSObject

@property (nonatomic, assign) float* data;
@property (nonatomic, assign) int length;
@property (nonatomic, assign) float* data2;
@property (nonatomic, assign) int length2;

@end

@interface MarsPositionPathModifierExtension : NSObject <MarsNativeExtension>

/**
 * params: 修改path的item列表
 */
- (instancetype)initWithParams:(NSArray<NSString*>*)params;

/**
 * 业务自行实现，获取动画相关参数
 */
- (void)onSceneDataParamsPixelRatio:(float)pixelRatio
                             Aspect:(float)aspect
                          AnimWidth:(float)animWidth
                         AnimHeight:(float)animHeight;

/**
 * 业务自行实现，修改path数据。type当前只有bezier曲线
 */
- (MarsPositionPathModifyResult*)modifyItemPathData:(NSString*)itemName
                                               Type:(NSString*)type
                                              Value:(float*)value
                                             Length:(int)length
                                             Value2:(float*)value2
                                            Length2:(int)length2;

@end

NS_ASSUME_NONNULL_END
