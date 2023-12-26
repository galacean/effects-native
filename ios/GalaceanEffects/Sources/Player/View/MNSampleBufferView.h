#import "MNBaseRenderView.h"
#import <CoreMedia/CoreMedia.h>
#import <AVFoundation/AVFoundation.h>

NS_ASSUME_NONNULL_BEGIN

// 和MNTextureView平级，节省view内存的一种方式
@interface MNSampleBufferView : UIView <MNBaseRenderView>

- (instancetype)initWithFrame:(CGRect)frame Scale:(CGFloat)screenScale;

- (CALayer *)getViewLayer;

@end

// 管理存储CVPixelBuffer和CVOpenGLESTexture
@interface MNSampleBufferTexture : NSObject

- (instancetype)initWithContext:(EAGLContext*)context Width:(int)width Heiht:(int)height;

- (CMSampleBufferRef)createSampleBuffer;

- (uint32_t)getGLTexture;

- (void)releaseTexture;

@end

NS_ASSUME_NONNULL_END
