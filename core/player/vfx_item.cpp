#include "vfx_item.h"
#include "plugin/types.h"
#include "player/composition.h"

namespace mn {

VFXItem::VFXItem() {
    transform_ = std::make_shared<Transform>();
}

VFXItem::~VFXItem() {
    MN_SAFE_RELEASE_CONTENT(content_);
}

void VFXItem::Create(ItemData* arg, Composition* comp, bool is_comp) {
    composition_ = comp;
    id_ = arg->id;
    name_ = arg->name;
    reusable_ = arg->reusable;
    delay_ = arg->delay;
    {
        if (arg->transform) {
            TransformData opt;
            if (arg->transform->position) {
                auto p = arg->transform->position;
                opt.position_ = new Vec3(p[0], p[1], p[2]);
            }
            if (arg->transform->rotation) {
                auto r = arg->transform->rotation;
                opt.SetRotationData(r[0], r[1], r[2]);
            }
            if (arg->transform->scale) {
                auto s = arg->transform->scale;
                opt.scale_ = new Vec3(s[0], s[1], s[2]);
            }
            if (arg->transform->quat) {
                auto q = arg->transform->quat;
                opt.quat_ = new Vec4(q[0], q[1], q[2], q[3]);
            }
            transform_->SetTransform(&opt);
        }
    }
    parent_id_ = arg->parent_id;
    time_in_ms_ = 0;
    duration_ = arg->duration;
    delay_in_ms_ = delay_ * 1000;
    dur_in_ms_ = duration_ * 1000;
    end_behavior_ = CreateEndBehaviorType(arg->end_behavior ? arg->end_behavior->val : 0);
    lifetime_ = -(delay_in_ms_ / dur_in_ms_);
    list_index_ = arg->list_index;
    transform_->SetParentTransform(comp->root_transform_);
    
    this->OnConstructed(arg);
    
    if (duration_ <= 0) {
        MLOGE("item duration %f invalid", duration_);
        assert(0);
    }
}

void VFXItem::Start() {
    if (!started_ || ended_) {
        started_ = true;
        delaying_ = true;
        time_in_ms_ = 0;
        call_end_ = false;
        ended_ = false;
    }
}

void VFXItem::Reset() {
    if (composition_) {
        OnItemRemoved(composition_, content_);
        MN_SAFE_RELEASE_CONTENT(content_);
    }
    started_ = false;
}

void VFXItem::InnerOnUpdate(float dt, std::shared_ptr<VFXItem> thiz) {
    if (skip_update_) {
        return;
    }
    if (started_ && !frozen_) {
        float time = (time_in_ms_ += dt);
        float now = time - delay_in_ms_;
        transform_->Invalid();
        if (delaying_ && now >= 0) {
            delaying_ = false;
            const auto& renderer = composition_;
            CreateContent();
            OnLifeTimeBegin(renderer, content_);
            composition_->ItemLifetimeBegin(thiz);
        }
        if (!delaying_) {
            float lifetime = now / dur_in_ms_;
            bool ended = IsEnded(now);
            bool should_update = true;
            if(ended) {
                should_update = false;
                if (!call_end_) {
                    call_end_ = true;
                    OnEnd();
                }
                if (end_behavior_ != END_BEHAVIOR_FORWARD) {
                    ended_ = true;
                    if (end_behavior_ == END_BEHAVIOR_PAUSE || end_behavior_ == END_BEHAVIOR_PAUSE_AND_DESTROY) {
                        composition_->PausePlayer(this);
                    } else if (end_behavior_ == END_BEHAVIOR_FREEZE) {
                        should_update = true;
                        lifetime = 1;
                    }
                    if (!reusable_) {
                        if (end_behavior_ == END_BEHAVIOR_DESTROY || end_behavior_ == END_BEHAVIOR_PAUSE_AND_DESTROY) {
                            Destroy(thiz);
                            return;
                        } else if (end_behavior_ == END_BEHAVIOR_PAUSE) {
//                                end_behavior_ = END_BEHAVIOR_FORWARD;
                        }
                    }
                    lifetime = fmin(lifetime, 1);
                } else {
                    should_update = true;
                    if (looping_) {
                        lifetime = lifetime - floor(lifetime);
                        if (lifetime < 0) {
                            lifetime += 1;
                        }
                    }
                }
            }
            lifetime_ = lifetime;
            if (should_update) {
                OnItemUpdate(dt, lifetime);
            }
        }
    }
}

void VFXItem::Stop() {
    InnerStop();
    started_ = false;
}

void VFXItem::Destroy(std::shared_ptr<VFXItem> thiz) {
    if (composition_) {
        composition_->DestroyItem(thiz);
        Reset();
        skip_update_ = true;
        composition_ = nullptr;
    }
}

}
