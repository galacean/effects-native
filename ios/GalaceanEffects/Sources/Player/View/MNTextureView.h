#import "MNBaseRenderView.h"

NS_ASSUME_NONNULL_BEGIN

@interface MNTextureView : UIView <MNBaseRenderView>

- (instancetype)initWithFrame:(CGRect)frame Scale:(CGFloat)screenScale;

- (CALayer *)getViewLayer;

@end

NS_ASSUME_NONNULL_END
