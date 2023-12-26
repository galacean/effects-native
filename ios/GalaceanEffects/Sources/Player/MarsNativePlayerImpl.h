#import <UIKit/UIKit.h>
#import "MarsNativePlayer.h"
#import "MarsDataBase.h"

NS_ASSUME_NONNULL_BEGIN

@class MarsNativeBuilder;

@interface MarsNativePlayerImpl : MarsNativePlayer

- (instancetype)initWithBuilder:(MarsNativeBuilder*)builder;

/// 检查是否降级。返回降级原因，nil表示不降级
- (NSString*)checkDowngrade;

- (void)initWithMarsData:(MarsDataBase*)marsData Complete:(void(^)(bool, NSError*))complete;

- (void)setPlaceHolderBitmap:(UIImage*)bitmap;

- (void)setDowngrade;

- (NSString*)getSourceId;

+ (int)generatePlayerIndex;

+ (NSString*)getFormattedSourceId:(NSString*)url;

@end

NS_ASSUME_NONNULL_END
