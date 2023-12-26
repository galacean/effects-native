//
//  EAGLSurface.m
//
//  Created by Zongming Liu on 2021/11/9.
//

#import "EAGLSurface.h"
#import "MNBaseRenderView.h"
#import "MNSampleBufferView.h"
#import "MarsTaskScheduleUtil.h"
#include "util/log_util.hpp"

#define CHECKCONTEXT \
if ([EAGLContext currentContext] != _context) { \
    [EAGLContext setCurrentContext:_context]; \
}


static int s_support_compressed_texture_ = -1;

static int s_gles_version_ = -1;

@implementation EAGLSurfaceConfig

- (instancetype) init {
    if (self = [super init]) {
        _sharecontext = nil;
        _renderingApi = kEAGLRenderingAPIOpenGLES2;
        _size = CGSizeMake(0, 0);
        _drawable = nil;
    }
    
    return self;
}

@end

@interface EAGLSurface ()
{
    GLuint _defaultFrameBuffer;
    GLuint _defaultColorRenderBuffer;
    GLuint _depthStencilRenderBuffer;
    
    GLint _drawableWidth;
    GLint _drawableHeight;
    
    GLenum _fboStatus;
    
    bool _AVQueuedSampleBufferRenderingStatusFailed;
}

@property (nonatomic, strong) MNSampleBufferTexture* sampleBufferTexture;
@property (nonatomic, assign) BOOL useSampleBufferLayer;

@end

@implementation EAGLSurface

+ (EAGLContext *)createEAGLContext:(EAGLContext *)shareContext {
    EAGLContext *context = NULL;
    
    s_gles_version_ = 0;
    if (shareContext) {
        MLOGD("use sharecontext: %p", shareContext);
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3 sharegroup:shareContext.sharegroup];
        s_gles_version_ = 3;
        if (!context) {
            s_gles_version_ = 2;
            context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2 sharegroup:shareContext.sharegroup];
        }
    } else {
        MLOGD("not use sharecontext");
        context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
        s_gles_version_ = 3;
        if (!context) {
            s_gles_version_ = 2;
            context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        }
    }
    
    return context;
}

- (instancetype) initWithConfig:(EAGLSurfaceConfig *)config {
    if (self = [super init]) {
        _config = config;
        
        _defaultFrameBuffer = 0;
        _depthStencilRenderBuffer = 0;
        _defaultColorRenderBuffer = 0;
        _AVQueuedSampleBufferRenderingStatusFailed = false;
        
        self.useSampleBufferLayer = !([_config.drawable isKindOfClass:[CAEAGLLayer class]]);
        
        // todo:mass
        
        EAGLContext* c = [EAGLContext currentContext];
        
        [self createContext];
        if (_context) {
            [self setupBuffers];
        }
        
        [EAGLContext setCurrentContext:c];
    }
    
    return self;
}

- (void) createContext {
    _context = [EAGLSurface createEAGLContext:_config.sharecontext];
}

- (void) setupBuffers {
    CHECKCONTEXT
    // 1. Frame Buffer;
    BOOL success = [self setupFrameBuffer];
    if (!success) {
        [EAGLContext setCurrentContext:nil];
        return ;
    }
    
    // 2. Render Buffer
    success = [self setupRenderBufferWithLayer];
    if (!success) {
        [EAGLContext setCurrentContext:nil];
        return ;
    }
    
    // 3. Depth Stencil Buffer;
    success = [self setupDepthStencilBuffer];
    if (!success) {
        [EAGLContext setCurrentContext:nil];
        return ;
    }
    
    // 4. check framebuffer status
    success = [self checkFBOStatus];
    if (!success) {
        [EAGLContext setCurrentContext:nil];
        return ;
    }
    
    MLOGD("set FrameBuffer success");
    return ;
}

- (BOOL) setupFrameBuffer {
    BOOL success = YES;
    GL_CHECK(glGenFramebuffers(1, &_defaultFrameBuffer));
    MLOGD("default frameBuffer id:%d", _defaultFrameBuffer);
    
    if (_defaultFrameBuffer <= 0) {
        success = NO;
        MLOGE("GenFrameBuffer failed");
    }
    
    return success;
}

- (BOOL) setupRenderBufferWithLayer {
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, _defaultFrameBuffer));
    if (self.useSampleBufferLayer) {
        CGFloat screenScale = [[UIScreen mainScreen] scale];
        int width = (int) (_config.size.width * screenScale);
        int height = (int) (_config.size.height * screenScale);
        
        self.sampleBufferTexture = [[MNSampleBufferTexture alloc] initWithContext:_context Width:width Heiht:height];
        _defaultColorRenderBuffer = [self.sampleBufferTexture getGLTexture];
        MLOGD("default renderTexture id: %d", _defaultColorRenderBuffer);
        
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, _defaultColorRenderBuffer));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _defaultColorRenderBuffer, 0));
        
        _drawableWidth = width;
        _drawableHeight = height;
    } else {
        GL_CHECK(glGenRenderbuffers(1, &_defaultColorRenderBuffer));
        MLOGD("default renderBuffer id: %d", _defaultColorRenderBuffer);
        
        GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, _defaultColorRenderBuffer));
        BOOL success = [self doRenderBufferStorage];
        if (!success) {
            MLOGD("setup renderBufferStorage failed");
            return NO;
        }
        
        GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, _defaultColorRenderBuffer));
        
        GL_CHECK(glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, (GLint*)&_drawableWidth));
        GL_CHECK(glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, (GLint*)&_drawableHeight));
    }
    
    MLOGD("drawable size:(%d, %d)", _drawableWidth, _drawableHeight);
    return YES;
}

- (BOOL) setupDepthStencilBuffer {
    // todo: 根据动画类型决定是否开stencil、depth
    GL_CHECK(glGenRenderbuffers(1, &_depthStencilRenderBuffer));
    if (_depthStencilRenderBuffer <= 0) {
        MLOGE("setup depthstencilbuffer failed");
        return NO;
    }
    GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, _depthStencilRenderBuffer));
    // todo: msaa
    GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8_OES, _drawableWidth, _drawableHeight));
    // bind depth
    GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, _depthStencilRenderBuffer));
    // bind stencil
    GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, _depthStencilRenderBuffer));
    return YES;
}

- (BOOL) checkFBOStatus {
    GLint bindFBO = 0;
    GL_CHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &bindFBO));
    GL_CHECK(_fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER));
    MLOGD("glCheckFramebufferStatus:%x", _fboStatus);
    if (_fboStatus == GL_FRAMEBUFFER_COMPLETE && bindFBO > 0) {
        MLOGD("create fbo success");
        _isFboCompleted = YES;
    } else {
        _isFboCompleted = NO;
        MLOGE("create fbo fail, glerror:%d", glGetError());
        return NO;
    }
    return YES;
}

- (BOOL) doRenderBufferStorage {
    __block BOOL success = YES;
    
    if (!_config.drawable) {
        GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_RGBA4, _config.size.width, _config.size.height));
        return YES;
    }
    
    if ([NSThread isMainThread]) {
        GL_CHECK(success = [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)_config.drawable]);
    } else {
        // todo: 同步等待有死锁风险
        double beginTime = CFAbsoluteTimeGetCurrent();
        dispatch_sync(dispatch_get_main_queue(), ^{
            GL_CHECK(success = [_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:(CAEAGLLayer *)_config.drawable]);
        });
        [EAGLContext setCurrentContext:_context];
        double endTime = CFAbsoluteTimeGetCurrent();
        if (endTime - beginTime > 5) {
            NSLog(@"sync renderBufferStorage in mainthread cost to long");
        }
    }
    
    return success;
}

- (void) updateWithConfig:(EAGLSurfaceConfig *)config {
    
}

- (void) resize:(CGSize)size {
    if (_config.size.width == size.width && _config.size.height == size.height) {
        MLOGD("EAGLSurface Resize skip");
        return;
    }
    MLOGD("EAGLSurface Resize %.2fx%.2f -> %.2fx%.2f", _config.size.width, _config.size.height, size.width, size.height);
    _config.size = size;
    [self destroy];
    
    if (!_context) {
        MLOGD("EAGLSurface context is null, recreate context");
        [self createContext];
    }
    
    if (_context) {
        [self setupBuffers];
    }
}

- (BOOL) display {
    if (!_config.drawable) {
        GL_CHECK(glFinish());
        return YES;
    }
    BOOL ret = YES;
    if (self.useSampleBufferLayer) {
        GL_CHECK(glFlush());
        
        AVSampleBufferDisplayLayer* sampleLayer = (AVSampleBufferDisplayLayer*)_config.drawable;
        if (sampleLayer.status == AVQueuedSampleBufferRenderingStatusFailed) {
            if (!_AVQueuedSampleBufferRenderingStatusFailed) { // 过滤日志，防止一直打印
                MLOGD("AVQueuedSampleBufferRenderingStatusFailed");
                _AVQueuedSampleBufferRenderingStatusFailed = true;
            }
            [MarsTaskScheduleUtil postToUIThread:^{
                [sampleLayer flush];
            } Delayd:0];
        } else {
            CMSampleBufferRef sampleBuffer = [self.sampleBufferTexture createSampleBuffer];
            if (sampleBuffer) {
                [sampleLayer enqueueSampleBuffer:sampleBuffer];
                CFRelease(sampleBuffer);
            }
        }
    } else {
        GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, _defaultColorRenderBuffer));
        ret = [_context presentRenderbuffer:GL_RENDERBUFFER];
        if (!ret) {
            MLOGE("presentRenderBuffer failed");
        }
    }
    
    return ret;
}

- (BOOL) makeCurrent {
    if (!_context) {
        return NO;
    }
    
    CHECKCONTEXT
    return YES;
}

- (BOOL) isValid {
    return _context != nil && _isFboCompleted;
}

- (GLuint) getDefaultFrameBufferId {
    return _defaultFrameBuffer;
}

- (void) destroy {
    MLOGD("EAGLSurface begin destroy");
    
    // default
    GL_CHECK(glDeleteFramebuffers(1, &_defaultFrameBuffer));
    _defaultFrameBuffer = 0;
    if (self.useSampleBufferLayer) {
        // do nothing
    } else {
        GL_CHECK(glDeleteRenderbuffers(1, &_defaultColorRenderBuffer));
    }
    _defaultColorRenderBuffer = 0;
    GL_CHECK(glDeleteRenderbuffers(1, &_depthStencilRenderBuffer));
    _depthStencilRenderBuffer = 0;
    
    if (self.sampleBufferTexture) {
        [self.sampleBufferTexture releaseTexture];
        self.sampleBufferTexture = nil;
    }
    
    [EAGLContext setCurrentContext:nil];
}


- (GLint) getDrawableWidth {
    return _drawableWidth;
}

- (GLint) getDrawableHeight {
    return _drawableHeight;
}

- (bool) isSupportCompressedTexture {
    if (s_support_compressed_texture_ != -1) {
        return s_support_compressed_texture_ == 1;
    }
    EAGLContext* c = [EAGLContext currentContext];
    CHECKCONTEXT
    
    s_support_compressed_texture_ = 0;
    const char* extensions = (const char*) glGetString(GL_EXTENSIONS);
    if (extensions) {
        NSString* temp = [NSString stringWithUTF8String:extensions];
        if ([temp containsString:@"GL_KHR_texture_compression_astc_hdr"] || [temp containsString:@"GL_KHR_texture_compression_astc_ldr"]) {
            s_support_compressed_texture_ = 1;
        }
    }
    
    [EAGLContext setCurrentContext:c];
    
    return s_support_compressed_texture_ == 1;
}

- (int) getGLESVersion {
    return s_gles_version_;
}

@end
