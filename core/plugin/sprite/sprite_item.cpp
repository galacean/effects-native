#include "sprite_item.h"
#include "sprite_vfx_item.h"
#include "sprite_mesh.h"
#include "util/color_utils.hpp"
#include "util/log_util.hpp"
#include "math/math_util.hpp"
#include "math/translate.h"
#include "util/vfx_value_getter.h"
#include "ri/render/mars_texture.hpp"
#include "player/composition.h"
#include "filters/filter_define.hpp"
#include "extensions/morph/custom_morph.hpp"

namespace mn {

static void GetAnchorOffset(Vec2& out, const Vec2& anchor, const std::shared_ptr<Transform> transform) {
    const Vec3* const scale = transform->scale_;
    out.Set(anchor.m[0] * scale->m[0], anchor.m[1] * scale->m[1]);
}

class SpriteTranslateTarget : public TranslateTarget {
public:
    SpriteTranslateTarget(SpriteItem* sprite) : sprite_(sprite) {
    }
    
    ValueGetter* GetSpeedOverLifetime() override {
        return sprite_->speed_over_lifetime_;
    }

    ValueGetter* GetGravityModifier() override {
        return sprite_->options_->gravity_modifier;
    }

    LinearVelOverLifetime* GetLinearVelOverLifetime() override {
        return sprite_->linear_vel_over_lifetime_;
    }
    
    OrbitalVelOverLifetime* GetOrbitalVelOverLifetime() override {
        return sprite_->orbita_vel_over_lifetime_;
    }

private:
    SpriteItem* sprite_;
};

SpriteItemPositionOverLifetime::SpriteItemPositionOverLifetime(ItemPositionOverLifetimeData* data) {
    if (data) {
        if (data->path_arr) {
            float* arr = data->path_arr;
            Vec3 temp(arr[0], arr[1], arr[2]);
            path_vec = new Vec3(temp);
        } else if (data->path_value) {
            path_value_ref = data->path_value;
        }
    }
}

SpriteItem::SpriteItem(SpriteContentData* props, MeshRendererOptions* opt, VFXItem* vfx_item) {
    if (vfx_item->transform_) {
        inner_transform_ = vfx_item->transform_;
    }
    
    position_over_lifetime_ = new SpriteItemPositionOverLifetime(props->position_over_lifetime);
    SetOption(props, vfx_item);
//    SetTransform(props);
    SetRenderInfo(props, opt);
    cache_.start_size.Set(*inner_transform_->scale_);
    SetColorOverLifetime(props);
    SetSizeOverLifetime(props);
    SetVelOverLifetime(props);
    SetRotOverLifetime(props);
    // filter
    
    time_ = 0;
    max_time_ = options_->duration;
    gravity_modifier_ = options_->gravity_modifier;
    SetSplits(props);
    
    if (vfx_item->list_index_) {
        list_index_ = vfx_item->list_index_;
    }
    
    if (props->texture_sheet_animation) {
        texture_sheet_animation_ = new SpriteItemTextureSheet;
        const auto& t = props->texture_sheet_animation;
        if (t->col) {
            texture_sheet_animation_->col = t->col->val;
        }
        if (t->row) {
            texture_sheet_animation_->row = t->row->val;
        }
        if (t->total) {
            texture_sheet_animation_->total = t->total->val;
        }
        if (t->animate) {
            texture_sheet_animation_->animate = t->animate->val;
        }
    }
    
    {
        const char* temp = vfx_item->GetCachePrefix();
        cache_prefix_ = temp ? temp : "-";
    }
    parent_id_ = vfx_item->parent_id_;
    reusable_ = vfx_item->reusable_;
    name_ = vfx_item->name_;
    
    render_info_ = SpriteMesh::GetImageItemRenderInfo(this);
    
    delay_ = vfx_item->delay_;
    hide_ = vfx_item->hide_;
}

SpriteItem::~SpriteItem() {
    MN_SAFE_DELETE(renderer_);
    MN_SAFE_DELETE(options_);
    MN_SAFE_DELETE(transform_);
    MN_SAFE_DELETE(opacity_over_lifetime_);
    MN_SAFE_DELETE(size_x_over_lifetime_);
    MN_SAFE_DELETE(size_y_over_lifetime_);
    MN_SAFE_DELETE(size_z_over_lifetime_);
    MN_SAFE_DELETE(linear_vel_over_lifetime_);
    MN_SAFE_DELETE(orbita_vel_over_lifetime_);
    MN_SAFE_DELETE(speed_over_lifetime_);
    MN_SAFE_DELETE(rotation_over_lifetime_);
    MN_SAFE_DELETE(texture_sheet_animation_);
    MN_SAFE_DELETE(render_info_);
    MN_SAFE_DELETE(will_translate_);
    MN_SAFE_DELETE(velocity_);
    MN_SAFE_DELETE(inner_geo_data_);
    MN_SAFE_DELETE(custom_motph_);
    MN_SAFE_DELETE(anchor_);
    MN_SAFE_DELETE(position_over_lifetime_);
}

SpriteRenderData* SpriteItem::GetRenderData(float _time, bool init) {
    Vec3 size_inc(1.0, 1.0, 1.0);
    Vec3 rot_inc(0.0, 0.0, 0.0);
    bool size_changed = false;
    bool rot_changed = false;
    bool color_changed = false;
    Vec4 color_inc(1.0, 1.0, 1.0, 1.0);
    float time = time_ < 0 ? time_ : fmaxf(time_, 0);
    float life_time = time / options_->duration;
    
    FallbackAnchor();
    SpriteRenderData* ret = new SpriteRenderData;
    ret->life = life_time;
    ret->transform = inner_transform_;
    
    life_time = life_time < 0 ? 0 : (life_time > 1 ? 1 : life_time);
    
    if (size_x_over_lifetime_) {
        size_inc.m[0] = size_x_over_lifetime_->GetValue(life_time);
        if (size_separate_axes_) {
            size_inc.m[1] = size_y_over_lifetime_->GetValue(life_time);
            size_inc.m[2] = size_z_over_lifetime_->GetValue(life_time);
        } else {
            size_inc.m[2] = size_inc.m[1] = size_inc.m[0];
        }
        size_changed = true;
    }
    
    if (size_changed || init) {
        ret->transform->SetScale(size_inc.m[0], size_inc.m[1], size_inc.m[2]);
    }
    
    ret->start_size = new Vec3(cache_.start_size);
    
    if (rotation_over_lifetime_) {
        if (rotation_over_lifetime_->separate_axes) {
            if (rotation_over_lifetime_->as_rotation) {
                rot_inc.m[0] = rotation_over_lifetime_->x->GetValue(life_time);
                rot_inc.m[1] = rotation_over_lifetime_->y->GetValue(life_time);
                rot_inc.m[2] = rotation_over_lifetime_->z->GetValue(life_time);
            } else {
                rot_inc.m[0] = rotation_over_lifetime_->x->GetIntegrateValue(0, life_time, 1.0);
                rot_inc.m[1] = rotation_over_lifetime_->y->GetIntegrateValue(0, life_time, 1.0);
                rot_inc.m[2] = rotation_over_lifetime_->z->GetIntegrateValue(0, life_time, 1.0);
            }
        } else {
            rot_inc.m[2] = rotation_over_lifetime_->as_rotation ?
                rotation_over_lifetime_->z->GetValue(life_time) :
                rotation_over_lifetime_->z->GetIntegrateValue(0, life_time, 1);
        }
        rot_changed = true;
    }
    
    if (rot_changed || init) {
        rot_inc.Add(transform_->rotation);
        ret->transform->SetRotation(rot_inc.m[0], rot_inc.m[1], rot_inc.m[2]);
    }
    
    if (color_over_lifetime_.size() > 0) {
        ColorUtils::GetColorFromGradientStops(color_over_lifetime_, color_inc, life_time, true);
        color_changed = true;
    }
    if (opacity_over_lifetime_) {
        color_inc.m[3] *= opacity_over_lifetime_->GetValue(life_time);
        color_changed = true;
    }
    
    if (color_changed || init) {
        color_inc.Multi(options_->start_color);
        ret->color = new Vec4(color_inc);
    }
    
    Vec3* pos = nullptr;
    if (WillTranslate() || init) {
        pos = new Vec3;
        SpriteTranslateTarget target(this);
        Translate::CalculateTranslation(*pos, &target, options_->gravity, time, options_->duration, &transform_->position, GetVelocity());
    }
    if (transform_->path) {
        if (!pos) {
            pos = new Vec3(transform_->position);
        }
        Vec3 temp;
        transform_->path->GetValues(life_time, temp.m, 3);
        pos->Add(temp);
    }
    if (pos) {
        ret->transform->SetPosition(pos->m[0], pos->m[1], pos->m[2]);
    }
    MN_SAFE_DELETE(pos);
    
    auto& ta = texture_sheet_animation_;
    if (ta) {
        int total = ta->total;
        if (!total) {
            total = ta->col * ta->row;
        }
        float tex_rect_x = 0.0f;
        float tex_rect_y = 0.0f;
        float tex_rect_w = 1.0f;
        float tex_rect_h = 1.0f;
        float flip = 0;
        if (splits_.size() > 0) {
            auto& sp = splits_[0];
            flip = sp[4];
            tex_rect_x = sp[0];
            tex_rect_y = sp[1];
            if (flip) {
                tex_rect_w = sp[3];
                tex_rect_h = sp[2];
            } else {
                tex_rect_w = sp[2];
                tex_rect_h = sp[3];
            }
        }
        float dx, dy;
        if (flip) {
            dx = 1.0f / ta->row * tex_rect_w;
            dy = 1.0f / ta->col * tex_rect_h;
        } else {
            dx = 1.0f / ta->col * tex_rect_w;
            dy = 1.0f / ta->row * tex_rect_h;
        }
        Vec2 tex_offset;
        if (ta->animate) {
            int frame_index = std::round(life_time * (total - 1));
            int y_index = std::floor(frame_index / ta->col);
            int x_index = frame_index - y_index * ta->col;
            if (flip) {
                tex_offset.Set(dx * y_index, dy * (texture_sheet_animation_->col - x_index));
            } else {
                tex_offset.Set(dx * x_index, dy * (1 + y_index));
            }
        } else {
            tex_offset.Set(0, dy);
        }
        ret->tex_offset = new Vec4(
            tex_rect_x + tex_offset.m[0],
            tex_rect_h + tex_rect_y - tex_offset.m[1],
            dx, dy
        );
    } else if (init) {
        ret->tex_offset = new Vec4(0, 0, 1, 1);
    }
    ret->hide = hide_;
    
    if (custom_motph_) {
        ret->geo_data = custom_motph_->Generate(motph_lifetime_);
    }
    
    return ret;
}

void SpriteItem::UpdateTime(float global_time) {
    float time = global_time - delay_;
    float duration = options_->duration;
    if (time >= max_time_ && options_->looping) {
        float start = time - max_time_;
        while (start > duration) {
            start -= duration;
        }
        time_ = start;
    } else {
        bool freeze = options_->end_behavior == END_BEHAVIOR_FREEZE;
        time_ = freeze ? fmin(max_time_, time) : time;
    }
}

bool SpriteItem::GetEnded() {
    return (time_ > max_time_ && !options_->looping && !reusable_);
}

void SpriteItem::SetOption(SpriteContentData* props, VFXItem* vfx_item) {
    options_ = new SpriteItemOptions;
    
    options_->start_speed = 0;
    if (props->position_over_lifetime && props->position_over_lifetime->start_speed) {
        options_->start_speed = props->position_over_lifetime->start_speed->number_;
    }
    
    if (inner_transform_ && inner_transform_->scale_) {
        options_->start_size = inner_transform_->scale_->m[0];
    }
    if (!options_->start_size) {
        options_->start_size = 1;
    }
    
    if (inner_transform_ && inner_transform_->scale_) {
        float scale1 = inner_transform_->scale_->m[1];
        if (!scale1) {
            scale1 = 1;
        }
        options_->size_aspect = inner_transform_->scale_->m[0] / scale1;
    }
    if (!options_->size_aspect) {
        options_->size_aspect = 1;
    }
    
    if (props->options && props->options->start_color) {
        options_->start_color.Set(
            props->options->start_color->data_[0],
            props->options->start_color->data_[1],
            props->options->start_color->data_[2],
            props->options->start_color->data_[3]);
    } else {
        options_->start_color.Set(1, 1, 1, 1);
    }
    
    options_->duration = vfx_item->duration_;
    
    options_->looping = (vfx_item->end_behavior_ == END_BEHAVIOR_RESTART);
    
    if (props->position_over_lifetime && props->position_over_lifetime->gravity) {
        float* gravity = props->position_over_lifetime->gravity;
        options_->gravity.Set(gravity[0], gravity[1], gravity[2]);
    }
    
    if (props->position_over_lifetime && props->position_over_lifetime->gravity_modifier) {
        options_->gravity_modifier = VFXValueGetter::CreateValueGetter(props->position_over_lifetime->gravity_modifier);
    }
    
    if (props->position_over_lifetime && props->position_over_lifetime->direction) {
        float* direction = props->position_over_lifetime->direction;
        options_->direction.Set(direction[0], direction[1], direction[2]);
        MathUtil::Vec3Normolize(options_->direction);
    }
    
    options_->end_behavior = vfx_item->end_behavior_;
}

void SpriteItem::SetTransform(SpriteContentData* props) {
    transform_ = new SpriteItemTransform;
    
    {
        Vec3 temp;
        inner_transform_->GetRotation(temp);
        transform_->rotation.Set(temp);
    }
    if (inner_transform_->position_) {
        transform_->position.Set(*inner_transform_->position_);
    }
    if (props->position_over_lifetime) {
        if (props->position_over_lifetime->path_arr) {
            float* arr = props->position_over_lifetime->path_arr;
            Vec3 temp(arr[0], arr[1], arr[2]);
            transform_->position.Add(temp);
        } else if (props->position_over_lifetime->path_value) {
            transform_->path = VFXValueGetter::CreateObjectValueGetter(props->position_over_lifetime->path_value);
        }
    }
}

void SpriteItem::SetRenderInfo(SpriteContentData* props, MeshRendererOptions* opt) {
    renderer_ = new SpriteItemRenderer;
    
    renderer_->render_mode = RENDER_MODE_BILLBOARD;
    if (props->renderer && props->renderer->render_mode) {
        renderer_->render_mode = CreateRenderModeType(props->renderer->render_mode->val);
    }
    
    renderer_->particle_origin = PARTICLE_ORIGIN_CENTER;
    if (props->renderer && props->renderer->particle_origin) {
        renderer_->particle_origin = CreateParticleOriginType(props->renderer->particle_origin->val);
    }
    
    renderer_->blending = BLEND_MODE_ALPHA;
    if (props->renderer && props->renderer->blending) {
        renderer_->blending = CreateBlendModeType(props->renderer->blending->val);
    }
    
    if (props->renderer) {
        renderer_->texture = InnerInitTexture(props->renderer->texture, opt);
    }
    
    renderer_->occlusion = false;
    if (props->renderer && props->renderer->occlusion) {
        renderer_->occlusion = props->renderer->occlusion->val;
    }
    
    renderer_->transparent_occlusion =
        (props->renderer->transparent_occlusion && props->renderer->transparent_occlusion->val) ||
        (props->renderer->mask_mode && (props->renderer->mask_mode->val == MASK_MODE_WRITE_MASK));
    
    renderer_->side = SIDE_BOTH;
    if (props->renderer && props->renderer->side) {
        renderer_->side = CreateSideModeType(props->renderer->side->val);
    }
    
    if (props->renderer && props->renderer->shape) {
        renderer_->shape_ref = props->renderer->shape;
    }
    
    renderer_->mask = 0;
    if (props->renderer && props->renderer->mask) {
        renderer_->mask = props->renderer->mask->val;
    }
    
    renderer_->mask_mode = MASK_MODE_NONE;
    if (props->renderer && props->renderer->mask_mode) {
        renderer_->mask_mode = CreateMaskModeType(props->renderer->mask_mode->val);
    }
    
    renderer_->order = 0;
    
    if (props->renderer && props->renderer->anchor) {
        renderer_->anchor = new Vec2(*props->renderer->anchor);
    }
}

void SpriteItem::SetColorOverLifetime(SpriteContentData* props) {
    if (props->color_over_lifetime) {
        auto color_over_lifetime = props->color_over_lifetime;

        if (color_over_lifetime->opacity) {
            opacity_over_lifetime_ = VFXValueGetter::CreateValueGetter(color_over_lifetime->opacity);
        }

        if (color_over_lifetime->color) {
            auto color = color_over_lifetime->color;
            if (color->data_type_ == MDataType::ARRAY && color->value_->type_.compare("gradient") == 0) {
                VFXValueGetter::CreateGradientColor(color_over_lifetime_, color->value_);
            }
        }
    }
}

void SpriteItem::SetSizeOverLifetime(SpriteContentData* props) {
    auto size_over_lifetime = props->size_over_lifetime;
    if (size_over_lifetime) {
        size_separate_axes_ = size_over_lifetime->separate_axes ? size_over_lifetime->separate_axes->val : false;
        if (size_separate_axes_) {
            if (size_over_lifetime->x) {
                size_x_over_lifetime_ = VFXValueGetter::CreateValueGetter(size_over_lifetime->x);
            } else {
                size_x_over_lifetime_ = VFXValueGetter::CreateConstantValueGetter(1);
            }
            if (size_over_lifetime->y) {
                size_y_over_lifetime_ = VFXValueGetter::CreateValueGetter(size_over_lifetime->y);
            } else {
                size_y_over_lifetime_ = VFXValueGetter::CreateConstantValueGetter(1);
            }
            if (size_over_lifetime->z) {
                size_z_over_lifetime_ = VFXValueGetter::CreateValueGetter(size_over_lifetime->z);
            } else {
                size_z_over_lifetime_ = VFXValueGetter::CreateConstantValueGetter(1);
            }
        } else {
            if (size_over_lifetime->size) {
                size_x_over_lifetime_ = VFXValueGetter::CreateValueGetter(size_over_lifetime->size);
            } else {
                size_x_over_lifetime_ = VFXValueGetter::CreateConstantValueGetter(1);
            }
        }
    }
}

void SpriteItem::SetVelOverLifetime(SpriteContentData* props) {
    if (props->position_over_lifetime) {
        auto position_over_lifetime = props->position_over_lifetime;
        
        linear_vel_over_lifetime_ = new LinearVelOverLifetime;
        if (position_over_lifetime->linear_x) {
            linear_vel_over_lifetime_->x = VFXValueGetter::CreateValueGetter(position_over_lifetime->linear_x);
            linear_vel_over_lifetime_->enabled = true;
        }
        if (position_over_lifetime->linear_y) {
            linear_vel_over_lifetime_->y = VFXValueGetter::CreateValueGetter(position_over_lifetime->linear_y);
            linear_vel_over_lifetime_->enabled = true;
        }
        if (position_over_lifetime->linear_z) {
            linear_vel_over_lifetime_->z = VFXValueGetter::CreateValueGetter(position_over_lifetime->linear_z);
            linear_vel_over_lifetime_->enabled = true;
        }
        linear_vel_over_lifetime_->as_movement = false;
        if (position_over_lifetime->as_movement) {
            linear_vel_over_lifetime_->as_movement = position_over_lifetime->as_movement->val;
        }
        
        orbita_vel_over_lifetime_ = new OrbitalVelOverLifetime;
        if (position_over_lifetime->orbital_x) {
            orbita_vel_over_lifetime_->x = VFXValueGetter::CreateValueGetter(position_over_lifetime->orbital_x);
            orbita_vel_over_lifetime_->enabled = true;
        }
        if (position_over_lifetime->orbital_y) {
            orbita_vel_over_lifetime_->y = VFXValueGetter::CreateValueGetter(position_over_lifetime->orbital_y);
            orbita_vel_over_lifetime_->enabled = true;
        }
        if (position_over_lifetime->orbital_z) {
            orbita_vel_over_lifetime_->z = VFXValueGetter::CreateValueGetter(position_over_lifetime->orbital_z);
            orbita_vel_over_lifetime_->enabled = true;
        }
        if (position_over_lifetime->orbital_center) {
            orbita_vel_over_lifetime_->center = new Vec3(
                position_over_lifetime->orbital_center[0],
                position_over_lifetime->orbital_center[1],
                position_over_lifetime->orbital_center[2]);
        }
        orbita_vel_over_lifetime_->as_rotation = false;
        if (position_over_lifetime->as_rotation) {
            orbita_vel_over_lifetime_->as_rotation = position_over_lifetime->as_rotation->val;
        }
        
        if (position_over_lifetime->speed_over_lifetime) {
            speed_over_lifetime_ = VFXValueGetter::CreateValueGetter(position_over_lifetime->speed_over_lifetime);
        }
    }
}

void SpriteItem::SetRotOverLifetime(SpriteContentData* props) {
    auto rot = props->rotation_over_lifetime;
    if (rot) {
        rotation_over_lifetime_ = new SpriteItemRotationOverLifetime;
        
        rotation_over_lifetime_->as_rotation = false;
        if (rot->as_rotation) {
            rotation_over_lifetime_->as_rotation = rot->as_rotation->val;
        }
        
        rotation_over_lifetime_->separate_axes = false;
        if (rot->separate_axes) {
            rotation_over_lifetime_->separate_axes = rot->separate_axes->val;
        }
        
        if (rot->z) {
            rotation_over_lifetime_->z = VFXValueGetter::CreateValueGetter(rot->z);
        } else {
            rotation_over_lifetime_->z = VFXValueGetter::CreateConstantValueGetter(0);
        }
        
        if (rotation_over_lifetime_->separate_axes) {
            if (rot->x) {
                rotation_over_lifetime_->x = VFXValueGetter::CreateValueGetter(rot->x);
            } else {
                rotation_over_lifetime_->x = VFXValueGetter::CreateConstantValueGetter(0);
            }
            if (rot->y) {
                rotation_over_lifetime_->y = VFXValueGetter::CreateValueGetter(rot->y);
            } else {
                rotation_over_lifetime_->y = VFXValueGetter::CreateConstantValueGetter(0);
            }
        }
    }
}

void SpriteItem::SetSplits(SpriteContentData* props) {
    bool has_splits = false;
    if (props->splits) {
        auto splits = props->splits;
        size_t count = splits->splits.size();
        for (int i = 0; i < count; i++) {
            const std::vector<float>& split = splits->splits[i];
            splits_.push_back(split);
        }
        has_splits = true;
        
    }
    if (!has_splits) {
        std::vector<float> singleSplits {0.0, 0.0, 1.0, 1.0, 0.0};
        splits_.push_back(singleSplits);
    }
}

int SpriteItem::InitTexture(int texture, MeshRendererOptions* options) {
    MLOGE("!!!! SpriteItem::%s", __FUNCTION__);
    return -1;
}

bool SpriteItem::WillTranslate() {
    if (!will_translate_) {
        will_translate_ = new bool;
        *will_translate_ = false;

        if (linear_vel_over_lifetime_ && linear_vel_over_lifetime_->enabled) {
            *will_translate_ = true;
            return true;
        }
        if (orbita_vel_over_lifetime_ && orbita_vel_over_lifetime_->enabled) {
            *will_translate_ = true;
            return true;
        }
        if ((options_->gravity_modifier && !MathUtil::IsZeroVec3(options_->gravity)) ||
            (options_->start_speed && !MathUtil::IsZeroVec3(options_->direction))) {
            *will_translate_ = true;
            return true;
        }
    }
    return *will_translate_;
}

Vec3* SpriteItem::GetVelocity() {
    if (!velocity_) {
        velocity_ = new Vec3;
        MathUtil::Vec3Dot(*velocity_, options_->direction, options_->start_speed);
    }
    return velocity_;
}

std::shared_ptr<MarsTexture> SpriteItem::InnerInitTexture(MInt* texture, MeshRendererOptions* opt) {
    if (texture) {
        return opt->composition->GetTextureContainerById(texture->val);
    }
    auto tex = opt->empty_texture;
    tex->AssignRenderer(opt->renderer);
    return tex;
}

void SpriteItem::SetCustomMorph(CustomMorph* morph) {
    custom_motph_ = morph;
}

void SpriteItem::FallbackAnchor() {
    if (!anchor_) {
        const SpriteItemRenderer* const renderer = renderer_;
        // 2d coord, x ->left y->down
        Vec2* anchor = renderer->anchor ? new Vec2(*renderer->anchor) : nullptr;
        bool set_offset = true;
        if (!anchor && renderer->particle_origin) {
            anchor = new Vec2();
            Translate::ConvertParticleOrigin2Anchor(anchor, renderer->particle_origin);
            set_offset = false;
        }
        if (!anchor) {
            anchor = new Vec2(0.5f, 0.5f);
        }
        
        Vec3 rotation;
        inner_transform_->GetRotation(rotation);
        Vec3 position(*inner_transform_->position_);
        
        transform_ = new SpriteItemTransform;
        if (position_over_lifetime_->path_vec) {
            position.Add(*position_over_lifetime_->path_vec);
        } else if (position_over_lifetime_->path_value_ref) {
            transform_->path = VFXValueGetter::CreateObjectValueGetter(position_over_lifetime_->path_value_ref);
        }
        transform_->position.Set(position);
        transform_->rotation.Set(rotation);
        
        anchor_ = anchor ? new Vec2(anchor->m[0] - 0.5f, 0.5f - anchor->m[1]) : new Vec2();
        if (set_offset) {
            Vec2 offset;
            GetAnchorOffset(offset, *anchor_, inner_transform_);
            transform_->position.m[0] += offset.m[0];
            transform_->position.m[1] += offset.m[1];
        }
        delete anchor;
    }
}

void SpriteItem::GetStartSizeXY(float& x, float& y) {
    x = cache_.start_size.m[0];
    y = cache_.start_size.m[1];
}

void SpriteItem::GetAnchorXY(float& x, float& y) {
    x = anchor_->m[0];
    y = anchor_->m[1];
}

}
