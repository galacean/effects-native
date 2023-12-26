//
//  mars_video_texture.hpp
//  MarsNative
//
//  Created by changxing on 2023/9/27.
//  Copyright © 2023 Alipay. All rights reserved.
//

#ifndef mars_video_texture_hpp
#define mars_video_texture_hpp

#include "video_image_data.hpp"
#include "ri/render/mars_texture.hpp"

namespace mn {

class MarsVideoTexture : public MarsTexture {
public:
    MarsVideoTexture(MarsRenderer *renderer, const TextureOption& texture_option, VideoContext* vc);
    
    ~MarsVideoTexture() override;
    
    bool UpdateVideoFrame();

    void SeekTo(int ms);
    
    void Pause();
    
    void Resume();

    bool IsVideo() const override {
        return true;
    }
    
private:
    VideoContext* vc_ = nullptr;
    bool paused_ = false;
    
    int frame_buffer_ = -1;
};

}

#endif /* mars_video_texture_hpp */
