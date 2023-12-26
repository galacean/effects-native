//
//  mars_surface_ios.cpp
//
//  Created by Zongming Liu on 2021/11/8.
//

#include "MarsSurfaceIOS.h"
#include "platform/ios/EAGLSurface.h"
#import "MNBaseRenderView.h"
#include "util/log_util.hpp"


namespace mn {

class SystemSurfaceHolder {
public:
    EAGLSurface* surface_ = nil;
};

MarsSurfaceIOS::MarsSurfaceIOS() {
    
}

MarsSurfaceIOS::~MarsSurfaceIOS() {
    if (surface_) {
        [surface_->surface_ destroy];
        delete surface_;
    }
}

bool MarsSurfaceIOS::Create(const SystemSurface &system_surface, int32_t s_width, int32_t s_height) {
    surface_ = new SystemSurfaceHolder;
    if (system_surface.surface) {
        UIView<MNBaseRenderView>* view = (__bridge UIView<MNBaseRenderView>*)system_surface.surface;

        EAGLSurfaceConfig* config = [[EAGLSurfaceConfig alloc] init];
        config.size = view.frame.size;
        config.drawable = [view getViewLayer];
        surface_->surface_ = [[EAGLSurface alloc] initWithConfig:config];
    } else {
        // 没传surface就用离屏
        EAGLSurfaceConfig* config = [[EAGLSurfaceConfig alloc] init];
        config.size = CGSizeMake(s_width, s_height);
        config.drawable = nil;
        surface_->surface_ = [[EAGLSurface alloc] initWithConfig:config];
    }
    return [surface_->surface_ isValid];
}

void MarsSurfaceIOS::OnSurfaceResize(int32_t width, int32_t height) {
    if (surface_ && surface_->surface_) {
        MLOGD("OnSurface Resize width %d height %d", width, height);
        CGSize size = CGSizeMake(width, height);
        [surface_->surface_ resize:size];
    } else {
        MLOGE("Resize surface failed for surface is null");
    }
}

bool MarsSurfaceIOS::OnSurfaceCreated(void*) {
    return MakeCurrent() != 0 && IsValid();
}

void MarsSurfaceIOS::Destroy() {
    [surface_->surface_ destroy];
}

int MarsSurfaceIOS::MakeCurrent() {
    return [surface_->surface_ makeCurrent];
}

int MarsSurfaceIOS::SwapBuffer() {
    return [surface_->surface_ display];
}

int MarsSurfaceIOS::GetSurfaceWidth() {
    return [surface_->surface_ getDrawableWidth];
}

int MarsSurfaceIOS::GetSurfaceHeight() {
    return [surface_->surface_ getDrawableHeight];
}

int MarsSurfaceIOS::GetDefaultFramebufferId() {
    return [surface_->surface_ getDefaultFrameBufferId];
}

bool MarsSurfaceIOS::IsSupportCompressedTexture() {
    return [surface_->surface_ isSupportCompressedTexture];
}

bool MarsSurfaceIOS::IsValid() {
    return [surface_->surface_ isValid];
}

int MarsSurfaceIOS::GetGLESVersion() {
    return [surface_->surface_ getGLESVersion];
}

}
