#include "platform/egl/mars_surface.hpp"

namespace mn {

class EGLPBufferContext;

class MarsOffscreenSurface : public MarsSurface {
public:
    MarsOffscreenSurface();

    ~MarsOffscreenSurface();

    bool Create(const SystemSurface &system_surface, int32_t s_width, int32_t s_height) override;

    bool OnSurfaceCreated(void* surface) override { return true; }

    void OnSurfaceResize(int width, int height) override {}
        
    void Destroy() override {}
    
    int MakeCurrent() override;
    
    int SwapBuffer() override;
    
    int GetSurfaceWidth() override {
        return width_;
    }
    
    int GetSurfaceHeight() override {
        return height_;
    }

    int GetDefaultFramebufferId() override {
        return framebuffer_;
    }

    bool IsValid() override;

    bool IsSupportCompressedTexture() override {
        return false;
    }

    uint32_t GetColorAttachment() const {
        return color_attachment_;
    }

    int GetGLESVersion() override;

private:
    bool CreateFBO();

private:
    EGLPBufferContext* context_;

    int width_ = 1;
    int height_ = 1;

    uint32_t framebuffer_ = 0;
    uint32_t color_attachment_ = 0;
    uint32_t depth_stencil_attachment_ = 0;
};

}
