//
//  EAGLSurface.h
//
//  Created by Zongming Liu on 2021/11/9.
//

#import <Foundation/Foundation.h>
#import <UIKit/UIKit.h>

#include "ri/gl.h"

NS_ASSUME_NONNULL_BEGIN

@interface EAGLSurfaceConfig : NSObject

@property (nonatomic, strong) EAGLContext* sharecontext;
@property (nonatomic, assign) EAGLRenderingAPI renderingApi;
@property (nonatomic, assign) CGSize size;
@property (nonatomic, strong) CALayer *drawable;

@end

@interface EAGLSurface : NSObject

@property (nonatomic, strong, readonly) EAGLSurfaceConfig *config;
@property (nonatomic, strong, readonly) EAGLContext *context;
@property (nonatomic, assign, readonly) BOOL isFboCompleted;

+ (EAGLContext *)createEAGLContext:(EAGLContext *)shareContext;

- (instancetype) initWithConfig:(EAGLSurfaceConfig *)config;

- (void) updateWithConfig:(EAGLSurfaceConfig *)config;

- (void) resize:(CGSize)size;

- (BOOL) display;

- (BOOL) makeCurrent;

- (BOOL) isValid;

- (void) destroy;

- (GLuint) getDefaultFrameBufferId;

- (GLint) getDrawableWidth;

- (GLint) getDrawableHeight;

- (bool) isSupportCompressedTexture;

- (int) getGLESVersion;

@end

NS_ASSUME_NONNULL_END
