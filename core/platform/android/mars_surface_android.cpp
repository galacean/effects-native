#include "mars_surface_android.h"
#include "util/log_util.hpp"
#include "egl_window_context.h"

namespace mn {

MarsSurfaceAndroid::MarsSurfaceAndroid(bool enable_surface_scale) {
    context_ = new EGLWindowContext(enable_surface_scale);
    MLOGD("MarsSurfaceAndroid::MarsSurfaceAndroid context %p", context_);
}

MarsSurfaceAndroid::~MarsSurfaceAndroid() {
    if (context_) {
        delete context_;
        context_ = nullptr;
    }
    MLOGD("MarsSurfaceAndroid::Destruct");
}

bool MarsSurfaceAndroid::Create(const SystemSurface& system_surface, int32_t s_width, int32_t s_height) {
    bool result = context_->Init(system_surface.surface);
    MLOGD("MarsSurfaceAndroid::Create %d %p", result, context_);
    return result;
}

bool MarsSurfaceAndroid::OnSurfaceCreated(void* surface) {
    bool result = context_->Init(surface);
    MLOGD("MarsSurfaceAndroid::OnSurfaceCreated %d %p", result, context_);
    return result;
}

void MarsSurfaceAndroid::OnSurfaceResize(int width, int height) {
    context_->Resize(width, height);
    MLOGD("MarsSurfaceAndroid::OnSurfaceResize");
}
    
void MarsSurfaceAndroid::Destroy() {
    context_->Init(nullptr);
}

int MarsSurfaceAndroid::MakeCurrent() {
    return context_->MakeCurrent();
}

int MarsSurfaceAndroid::SwapBuffer() {
    return context_->SwapBuffers();
}

int MarsSurfaceAndroid::GetSurfaceWidth() {
    return context_->GetWidth();
}

int MarsSurfaceAndroid::GetSurfaceHeight() {
    return context_->GetHeight();
}

int MarsSurfaceAndroid::GetDefaultFramebufferId() {
    return 0;
}

bool MarsSurfaceAndroid::IsValid() {
    return context_ && context_->IsValid();
}

bool MarsSurfaceAndroid::IsSupportCompressedTexture() {
    return false;
}

bool MarsSurfaceAndroid::IsSurfaceless() {
    return context_ && context_->UsingPBuffer();
}

int MarsSurfaceAndroid::GetGLESVersion() {
    if (context_) {
        return context_->GetGLESVersion();
    }
    return 0;
}

}
