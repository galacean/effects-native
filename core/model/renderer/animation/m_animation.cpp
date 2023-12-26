//
//  m_animation.cpp
//
//  Created by Zongming Liu on 2022/10/9.
//

#include "m_animation.hpp"
#include "ri/render/mars_texture.hpp"
#include "ri/render/mars_renderer.hpp"

namespace mn {

MAnimTexture::MAnimTexture(MarsRenderer* renderer) : renderer_(renderer) {
    half_float_ = renderer_->GPUCapability()->SupportHalfFloatTexture();
}

MAnimTexture::~MAnimTexture() {
    
}

void MAnimTexture::Create(int joint_count, const std::string& name) {
    width_ = 4;
    height_ = joint_count;
    name_ = name;
    
    TextureOption option(width_, height_, TextureSourceType::DATA);
    option.texture_info.target = GL_TEXTURE_2D;
    if (half_float_) {
        option.texture_info.type = GL_HALF_FLOAT;
    } else {
        option.texture_info.type = GL_FLOAT;
    }
    
    option.texture_info.format = GL_RGBA;
    option.texture_info.internal_format = GL_RGBA;
    option.texture_info.mag_filter = GL_NEAREST;
    option.texture_info.min_filter = GL_NEAREST;
    option.texture_info.wrap_s = GL_CLAMP_TO_EDGE;
    option.texture_info.wrap_t = GL_CLAMP_TO_EDGE;
    
    texture_ = std::make_shared<MarsTexture>(renderer_, option);
}

void MAnimTexture::Update(uint8_t* buffer) {
    TextureRawData raw_data(width_, height_);
    raw_data.data = buffer;
    texture_->UpdateSource(raw_data);
}

MAnimationTrack::MAnimationTrack() {
    
}

MAnimationTrack::~MAnimationTrack() {
    
}

void MAnimationTrack::Create(std::shared_ptr<MarsAnimTrackOptions> option) {
    target_type_ = GetTarget(option->path);
    method_type_ = GetMethodType(option->interpolation);
    node_id_ = option->node;
    sampler_ = InterpolationSampler::CreateAnimationSampler(method_type_, target_type_, option->input, option->input_byte_length, option->output, option->output_byte_length, this->component_);
    
    end_time_ = ((float *)option->input)[(option->input_byte_length/(sizeof(float))) - 1];
}

void MAnimationTrack::Tick(float time,  std::shared_ptr<TreeVFXItem> tree_item) {
    if (tree_item && !tree_item->GetContent()) {
        // todo: 临时规避闪退
        DEBUG_MLOGD("!!!\n临时规避闪退\n!!!");
        return;
    }
    if (tree_item) {
        auto node = ((Tree*) tree_item->GetContent())->GetNodeById(std::to_string(node_id_));
        if (this->sampler_ && node) {
            float *result = this->sampler_->Evaluate(time);
            
            switch (this->target_type_) {
                case MAnimTargetType::TRANSLATION: {
                    node->transform->SetPosition(result[0], result[1], result[2]);
                    break;
                }
                case MAnimTargetType::ROTATION: {
                    node->transform->SetQuat(result[0], result[1], result[2], result[3]);
                    break;
                }
                case MAnimTargetType::SCALE: {
                    node->transform->SetScale(result[0], result[1], result[2]);
                    break;
                }
                default:
                    break;
            }
            
            free(result);
        }
    } else {
        DEBUG_MLOGD("Animation Track Failed Tree Item ");
    }
}

MAnimTargetType MAnimationTrack::GetTarget(const std::string& target) {
    if (target == "translation") {
        this->component_ = 3;
        return MAnimTargetType::TRANSLATION;
    } else if (target == "rotation") {
        this->component_ = 4;
        return MAnimTargetType::ROTATION;
    } else {
        this->component_ = 3;
        return MAnimTargetType::SCALE;
    }
}

MAnimMethodType MAnimationTrack::GetMethodType(const std::string& method) {
    if (method == "LINEAR") {
        return MAnimMethodType::LINEAR;
    } else if (method == "STEP") {
        return MAnimMethodType::STEP;
    } else {
        return MAnimMethodType::CUBICSPLINE;
    }
}

MAnimation::MAnimation() {
    
}

MAnimation::~MAnimation() {
    
}

void MAnimation::Create(std::shared_ptr<MarsAnimationOptions> opiton) {
    duration_ = 0.0f;
    loop_count_ = 0;
    anim_time_ = 0.0f;

    
    const auto& tracks = opiton->tracks;
    for (size_t i=0; i<tracks.size(); i++) {
        std::shared_ptr<MAnimationTrack> anim_track = std::make_shared<MAnimationTrack>();
        anim_track->Create(tracks[i]);
        this->anim_tracks_.push_back(anim_track);
        
        if (duration_ < anim_track->EndTime()) {
            duration_ = anim_track->EndTime();
        }
    }
}

void MAnimation::Tick(float dt, std::shared_ptr<TreeVFXItem> tree_item) {
    float anim_time = this->UpdateAnimTime(dt);
    for (size_t i=0; i<anim_tracks_.size(); i++) {
        anim_tracks_[i]->Tick(anim_time, tree_item);
    }
}

float MAnimation::UpdateAnimTime(float dt) {
    anim_time_ += dt;
    loop_count_ = std::floor(anim_time_ / duration_);
    float mode = fmod(anim_time_, duration_);
    return mode;
}

MAnimationManager::MAnimationManager() {
    
}

MAnimationManager::~MAnimationManager() {
    
}

void MAnimationManager::Create(std::shared_ptr<TreeOptionsAnimEx> tree_anim_ex, std::shared_ptr<TreeVFXItem> tree_item) {
    this->owner_item_ = tree_item;
    animation_speed_ = 1.0;
    default_animation_ = tree_anim_ex->animation;
    
    auto& animation_options = tree_anim_ex->animation_options;
    for (size_t i=0; i<animation_options.size(); i++) {
        auto m_animation = this->CreateAnimation(animation_options[i]);
        this->m_animations_.push_back(m_animation);
    }
}

std::shared_ptr<MAnimation> MAnimationManager::CreateAnimation(std::shared_ptr<MarsAnimationOptions> anim_option) {
    std::shared_ptr<MAnimation> animation = std::make_shared<MAnimation>();
    animation->Create(anim_option);
    return animation;
}

void MAnimationManager::Tick(float delta_seconds) {
    float new_delta_seconds = delta_seconds * this->animation_speed_ * 0.001;

    if (default_animation_ >= 0 && default_animation_ < m_animations_.size()) {
        auto m_animation = m_animations_[default_animation_];
        m_animation->Tick(new_delta_seconds, owner_item_);
    }
}

void MAnimationController::Create(std::vector<std::shared_ptr<TreeVFXItem>> tree_items) {
    managers_.clear();
    for (int i = 0; i < tree_items.size(); i++) {
        auto tree = tree_items[i];
        auto mgr = std::make_shared<MAnimationManager>();
        auto options = tree->options_->tree;
        mgr->Create(options, tree);
        managers_.push_back(mgr);
    }
}

void MAnimationController::Tick(float dt) {
    for (size_t i=0; i<managers_.size(); i++) {
        managers_[i]->Tick(dt);
    }
}

}
