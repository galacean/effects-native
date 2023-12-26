#import "MNTextureView.h"

@implementation MNTextureView

+ (Class)layerClass {
    return [CAEAGLLayer class];
}

- (instancetype)initWithFrame:(CGRect)frame Scale:(CGFloat)screenScale {
    if (self = [super initWithFrame:frame]) {
        CAEAGLLayer *eaglLayer = (CAEAGLLayer *)self.layer;
        eaglLayer.opaque = true;
        eaglLayer.drawableProperties = @{kEAGLDrawablePropertyColorFormat:kEAGLColorFormatRGBA8,
                                         kEAGLDrawablePropertyRetainedBacking : @(YES)};
        
        self.backgroundColor = [UIColor clearColor];
        
        eaglLayer.contentsScale = screenScale;
    }
    return self;
}

- (CALayer*)getViewLayer {
    return self.layer;
}

@end
