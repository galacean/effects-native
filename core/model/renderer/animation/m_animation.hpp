//
//  m_animation.hpp
//
//  Created by Zongming Liu on 2022/10/9.
//

#ifndef m_animation_hpp
#define m_animation_hpp

#include <stdio.h>
#include <string>
#include "model/common.hpp"
#include "model/renderer/animation/interpolation_sampler.hpp"
#include "plugin/tree/tree_vfx_item.hpp"

namespace mn {

class MAnimTexture {
    
public:
    
    MAnimTexture(MarsRenderer* renderer);
    
    ~MAnimTexture();
    
    void Create(int joint_count, const std::string& name);
    
    void Update(uint8_t* buffer);
    
    std::shared_ptr<MarsTexture> InnerTexture() {
        return texture_;
    }
    
private:
    
    int width_;
    
    int height_;
    
    std::string name_;
    
    std::shared_ptr<MarsTexture> texture_;
    
    bool half_float_ = false;
    
    MarsRenderer* renderer_;
    
};


class MAnimationTrack {
    
public:
    
    MAnimationTrack();
    
    ~MAnimationTrack();
    
    void Create(std::shared_ptr<MarsAnimTrackOptions> option);
    
    void Tick(float dt, std::shared_ptr<TreeVFXItem> tree_item);
    
    float EndTime() {
        return end_time_;
    }
    
private:
    
    MAnimTargetType GetTarget(const std::string& target);
    
    MAnimMethodType GetMethodType(const std::string& method);
    
    int node_id_;
    
    uint8_t* time_array_;
    
    size_t time_byte_length_;
    
    uint8_t* data_array_;
    
    size_t data_byte_length_;
    
    // 数据的大小vec3 or vec4;
    size_t component_ = 3;
    
    float end_time_;
    
    MAnimMethodType method_type_ = MAnimMethodType::LINEAR;
    
    MAnimTargetType target_type_ = MAnimTargetType::TRANSLATION;
    
    std::shared_ptr<InterpolationSampler> sampler_;

};

class MAnimation {
    
public:
    
    MAnimation();
    
    ~MAnimation();
    
    void Create(std::shared_ptr<MarsAnimationOptions> opiton);
    
    void Tick(float dt, std::shared_ptr<TreeVFXItem> tree_item);
    
    bool IsNodeAnim() {
        return is_node_anim_;
    }
    
private:
    
    float UpdateAnimTime(float dt);
        
    std::vector<std::shared_ptr<MAnimationTrack>> anim_tracks_;
    
    float duration_;
    
    float anim_time_;
    
    int loop_count_;
    
    bool is_node_anim_;

};

class MAnimationManager {
  
public:
    
    MAnimationManager();
    
    ~MAnimationManager();
    
    void Create(std::shared_ptr<TreeOptionsAnimEx> tree_anim_ex, std::shared_ptr<TreeVFXItem> tree_item);
            
    std::shared_ptr<MAnimation> CreateAnimation(std::shared_ptr<MarsAnimationOptions> anim_options);
    
    void Tick(float delta_seconds);
    
private:
    std::vector<std::shared_ptr<MAnimation>> m_animations_;
    
    std::shared_ptr<TreeVFXItem> owner_item_;

    float animation_speed_;
    
    int default_animation_;
};

class MAnimationController {
public:
    void Create(std::vector<std::shared_ptr<TreeVFXItem>> tree_items);
    
    void Tick(float dt);
    
    void Destroy() { assert(0); }
    
private:
    std::vector<std::shared_ptr<MAnimationManager>> managers_;
    
};

}

#endif /* m_animation_hpp */
