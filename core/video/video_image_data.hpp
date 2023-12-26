//
//  video_image_data.hpp
//  MarsNative
//
//  Created by changxing on 2023/9/26.
//  Copyright © 2023 Alipay. All rights reserved.
//

#ifndef video_image_data_hpp
#define video_image_data_hpp

#include "sceneData/scene_data_decoder.h"

namespace mn {

class VideoPlayerEngineListener;
class VideoBufferProcessor;
class MarsVideoPlayer;

using VideoPrepareCallback = std::function<void(bool, const std::string& msg)>;

class VideoContext {
public:
    VideoContext(const std::string& file_path, const std::string& key);
    
    ~VideoContext();
    
    void Prepare(bool transparent, bool hard_decoder, const VideoPrepareCallback& callback);
    
    int GetWidth() const;
    
    int GetHeight() const;
    
    bool RenderToFBO(int frame_buffer);

    void SeekTo(int ms);
    
    // todo: 生命周期事件如何解决长链路一路调过来的问题
    void Pause();
    
    void Resume();

    void Stop();

private:
    bool StartEngine();
    
private:
    std::unique_ptr<MarsVideoPlayer> engine_;
    
    std::shared_ptr<bool> is_destroyed_;
    
    std::string file_path_;
    
    std::string key_;
    
    std::shared_ptr<VideoPlayerEngineListener> engine_listener_;
    
    bool started_ = false;
};

struct VideoImageRawData : public ImageRawData {
    VideoImageRawData(VideoContext* video_context);
    
    ~VideoImageRawData() override;
    
    VideoContext* video_context_ = nullptr;
};

}

#endif /* video_image_data_hpp */
