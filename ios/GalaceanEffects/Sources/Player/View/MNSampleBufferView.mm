//
//  MNSimpleBufferView.m
//  MarsNative
//
//  Created by changxing on 2023/10/31.
//  Copyright © 2023 Alipay. All rights reserved.
//

#import "MNSampleBufferView.h"
#include "ri/backend/opengl/gl_api.hpp"
#include "util/log_util.hpp"

@implementation MNSampleBufferView

+ (Class)layerClass {
    // 这个view一定返回AVSampleBufferDisplayLayer
    return [AVSampleBufferDisplayLayer class];
}

- (instancetype)initWithFrame:(CGRect)frame Scale:(CGFloat)screenScale {
    if (self = [super initWithFrame:frame]) {
        AVSampleBufferDisplayLayer* sampleLayer = (AVSampleBufferDisplayLayer*) self.layer;
        sampleLayer.videoGravity = AVLayerVideoGravityResizeAspect;
        self.backgroundColor = [UIColor clearColor];
//        sampleLayer.contentsScale = screenScale;
        self.transform = CGAffineTransformMakeScale(1, -1); // 设置上下翻转
    }
    return self;
}

- (CALayer*)getViewLayer {
    return self.layer;
}

@end

@interface MNSampleBufferTexture ()

@property (nonatomic, assign) int width;
@property (nonatomic, assign) int height;
@property (nonatomic, assign) CVOpenGLESTextureCacheRef cvTextureCache;
@property (nonatomic, assign) CVOpenGLESTextureRef cvTexture;
@property (nonatomic, assign) CVPixelBufferRef pxBuffer;
@property (nonatomic, assign) uint32_t textureName;
@property (nonatomic, strong) EAGLContext* context;

@end

@implementation MNSampleBufferTexture

- (instancetype)initWithContext:(EAGLContext*)context Width:(int)width Heiht:(int)height {
    if (self = [super init]) {
        self.context = context;
        self.width = width;
        self.height = height;
        self.textureName = 0;
        
        [self setup];
    }
    return self;
}

- (CMSampleBufferRef)createSampleBuffer {
    if (!self.pxBuffer) {
        return NULL;
    }
    // 设置时间信息
    double currentTime = CFAbsoluteTimeGetCurrent();
    CMSampleTimingInfo timing = {kCMTimeInvalid, CMTimeMakeWithSeconds(currentTime, 1000), kCMTimeInvalid};
    // 获取视频信息
    CMVideoFormatDescriptionRef videoInfo = NULL;
    OSStatus result = CMVideoFormatDescriptionCreateForImageBuffer(NULL, _pxBuffer, &videoInfo);

    CMSampleBufferRef sampleBuffer = NULL;
    result = CMSampleBufferCreateForImageBuffer(kCFAllocatorDefault, _pxBuffer, true, NULL, NULL, videoInfo, &timing, &sampleBuffer);
    
    // set attachment
    if (sampleBuffer) {
        CFArrayRef attachments = CMSampleBufferGetSampleAttachmentsArray(sampleBuffer, YES);
        CFMutableDictionaryRef dict = (CFMutableDictionaryRef)CFArrayGetValueAtIndex(attachments, 0);
        if (dict) {
            CFDictionarySetValue(dict, kCMSampleAttachmentKey_DisplayImmediately, kCFBooleanTrue);
        }
    }

    if (videoInfo) {
        CFRelease(videoInfo);
    }
    return sampleBuffer;
}

- (uint32_t)getGLTexture {
    return self.textureName;
}

- (void)releaseTexture {
    MLOGD("MNSampleBufferTexture releaseTexture begin");
//    @property (nonatomic, assign) uint32_t textureName;
    if (_cvTextureCache) {
        CFRelease(_cvTextureCache);
        _cvTextureCache = nullptr;
    }
    if (_pxBuffer) {
        CVPixelBufferRelease(_pxBuffer);
        _pxBuffer = nullptr;
    }
    if (_cvTexture) {
        CFRelease(_cvTexture);
        _cvTexture = nullptr;
    }
    MLOGD("MNSampleBufferTexture releaseTexture end");
}

#pragma mark - private

// 创建CVOpenGLESTexture
- (void)setup {
    NSDictionary *options = @{
        (NSString*)kCVPixelBufferIOSurfacePropertiesKey: [NSDictionary dictionary]
    };
    CVReturn status = CVPixelBufferCreate(kCFAllocatorDefault, _width, _height, kCVPixelFormatType_32BGRA,
            (__bridge CFDictionaryRef)options, &_pxBuffer);
    if (status != kCVReturnSuccess || !_pxBuffer) {
        MLOGE("CVPixelBufferCreate fail %d %p", status, _pxBuffer);
        return;
    }
    status = CVOpenGLESTextureCacheCreate(kCFAllocatorDefault, NULL, self.context, NULL, &_cvTextureCache);
    if (status != kCVReturnSuccess) {
        MLOGE("CVOpenGLESTextureCacheCreate fail %d", status);
        return;
    }
    status = CVOpenGLESTextureCacheCreateTextureFromImage(
            kCFAllocatorDefault, _cvTextureCache, _pxBuffer, NULL, GL_TEXTURE_2D, GL_RGBA, _width,
            _height, GL_BGRA, GL_UNSIGNED_BYTE, 0, &_cvTexture);
    if (status != kCVReturnSuccess || !_cvTexture) {
        MLOGE("CVOpenGLESTextureCacheCreateTextureFromImage fail %d %p", status, _pxBuffer);
        return;
    }
    self.textureName = CVOpenGLESTextureGetName(_cvTexture);
    MLOGD("MNSampleBufferTexture genTex %u %dx%d", self.textureName, _width, _height);
}

@end
