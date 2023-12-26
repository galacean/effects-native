//
//  video_buffer_processor.hpp
//  MarsNative
//
//  Created by changxing on 2023/10/7.
//  Copyright © 2023 Alipay. All rights reserved.
//

#ifndef video_buffer_processor_hpp
#define video_buffer_processor_hpp

#include <mutex>
#include <CoreVideo/CoreVideo.h>

namespace mn {

class VideoBufferProcessor {
public:
    VideoBufferProcessor(bool transparent);
    
    ~VideoBufferProcessor();
    
    bool RenderToFBO(int frame_buffer);
    
    void ReleaseGLObject();
    
    static long OnSurfaceData(int option, void* view, void* data, int64_t intValue);
    
private:
    void Setup();
    
    void OnSurfaceDataInternal(void* data, int64_t intValue);
    
private:
    // 是否是透明视频
    bool transparent_ = false;
    
    int v_shader_ = -1;
    int f_shader_ = -1;
    int program_ = -1;
    
    int attr_pos_ = 0;
    int attr_uv_ = 0;
    int u_sample_y_ = 0;
    int u_sample_uv_ = 0;
    int u_matrix_ = 0;
    
    int gl_buffer_ = 0;
    
    bool setup_ = false;
    const float* conversion_ = nullptr;
    CVOpenGLESTextureCacheRef cv_texture_cache_ = nullptr;
    CVOpenGLESTextureRef y_tex_ = nullptr;
    CVOpenGLESTextureRef uv_tex_ = nullptr;
    
    std::mutex buffer_mutex_;
    CVPixelBufferRef pixel_buffer_ = nullptr;
};

}

#endif /* video_buffer_processor_hpp */
