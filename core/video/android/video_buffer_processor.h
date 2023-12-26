//
// Created by changxing on 2023/10/7.
//

#ifndef ANDROID_VIDEO_BUFFER_PROCESSOR_H
#define ANDROID_VIDEO_BUFFER_PROCESSOR_H

#include <android/native_window_jni.h>

namespace mn {

class VideoBufferProcessor {
public:
    VideoBufferProcessor(bool transparent);

    ~VideoBufferProcessor();

    bool RenderToFBO(int frame_buffer);

    ANativeWindow* CreateSurface(int width, int height);

    void ReleaseGLObject();

private:
    void ReleaseSurface();

    void Setup();

private:
    bool transparent_ = false;

    int v_shader_ = -1;
    int f_shader_ = -1;
    int program_ = 0;

    int attr_pos_ = 0;
    int attr_uv_ = 0;

    int gl_buffer_ = 0;

    bool setup_ = false;
    int width_ = 0;
    int height_ = 0;

    jobject surface_texture_ref_ = nullptr;
    ANativeWindow* native_window_ = nullptr;
    int video_texture_ = 0;
};

}

#endif //ANDROID_VIDEO_BUFFER_PROCESSOR_H
