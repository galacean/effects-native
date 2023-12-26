//
//  particle_system.cpp
//
//  Created by Zongming Liu on 2022/4/12.
//

#include "particle_system.hpp"
#include "particle_vfx_item.hpp"
#include "util/log_util.hpp"
#include "util/vfx_value_getter.h"
#include "math/value/linear_value_getter.hpp"
#include "math/translate.h"
#include "player/composition.h"

namespace mn {

ParticleSystem::ParticleSystem(MarsRenderer* renderer, ParticleContentData* props, MeshRendererOptions* renderer_opt, ParticleVFXItem* vfx_item) {
    auto options = props->options;
    auto position_over_lifetime = props->position_over_lifetime;
    auto shape = props->shape;
    auto duration = vfx_item->duration_;
    auto gravity_modifier = position_over_lifetime->gravity_modifier;
    SetTextureSheedAnimation(props);
    render_matrix_ = Mat4::IDENTITY;
    world_matrix_ = Mat4::IDENTITY;
    
    SetOptions(props, vfx_item);
    SetShapeOption(props);
    SetEmissionOption(props);
    
    name_ = vfx_item->name_;
    
    {
        const auto renderer = props->renderer;
        RotationOverLifeTimeData* rotation_over_lifetime = new RotationOverLifeTimeData;
        const auto rot_over_lt = props->rotation_over_lifetime;
        if (rot_over_lt) {
            rotation_over_lifetime->as_rotation = rot_over_lt->as_rotation && rot_over_lt->as_rotation->val;
            if (rot_over_lt->z) {
                rotation_over_lifetime->rot_z = VFXValueGetter::CreateValueGetter(rot_over_lt->z);
            }
            if (rot_over_lt->separate_axes && rot_over_lt->separate_axes->val) {
                if (rot_over_lt->x) {
                    rotation_over_lifetime->rot_x = VFXValueGetter::CreateValueGetter(rot_over_lt->x);
                }
                if (rot_over_lt->y) {
                    rotation_over_lifetime->rot_y = VFXValueGetter::CreateValueGetter(rot_over_lt->y);
                }
            }
        }
        
        ForceTargetData* force_target = nullptr;
        if (position_over_lifetime->force_target && position_over_lifetime->force_target->val) {
            force_target = new ForceTargetData;
            if (position_over_lifetime->target) {
                force_target->target.Set(
                    position_over_lifetime->target[0],
                    position_over_lifetime->target[1],
                    position_over_lifetime->target[2]);
            }
            if (position_over_lifetime->force_curve) {
                force_target->curve = VFXValueGetter::CreateValueGetter(position_over_lifetime->force_curve);
            } else {
                force_target->curve = new LinearValueGetter(0, 1);
            }
        }
        
        LinearVelOverLifetime* linear_vel_over_lifetime = new LinearVelOverLifetime;
        if (position_over_lifetime->linear_x) {
            linear_vel_over_lifetime->x = VFXValueGetter::CreateValueGetter(position_over_lifetime->linear_x);
        }
        if (position_over_lifetime->linear_y) {
            linear_vel_over_lifetime->y = VFXValueGetter::CreateValueGetter(position_over_lifetime->linear_y);
        }
        if (position_over_lifetime->linear_z) {
            linear_vel_over_lifetime->z = VFXValueGetter::CreateValueGetter(position_over_lifetime->linear_z);
        }
        linear_vel_over_lifetime->as_movement = position_over_lifetime->as_movement && position_over_lifetime->as_movement->val;
        
        OrbitalVelOverLifetime* orbital_vel_over_lifetime = new OrbitalVelOverLifetime;
        if (position_over_lifetime->orbital_x) {
            orbital_vel_over_lifetime->x = VFXValueGetter::CreateValueGetter(position_over_lifetime->orbital_x);
        }
        if (position_over_lifetime->orbital_y) {
            orbital_vel_over_lifetime->y = VFXValueGetter::CreateValueGetter(position_over_lifetime->orbital_y);
        }
        if (position_over_lifetime->orbital_z) {
            orbital_vel_over_lifetime->z = VFXValueGetter::CreateValueGetter(position_over_lifetime->orbital_z);
        }
        if (position_over_lifetime->orbital_center) {
            float* c = position_over_lifetime->orbital_center;
            orbital_vel_over_lifetime->center = new Vec3(c[0], c[1], c[2]);
        }
        if (position_over_lifetime->as_rotation) {
            orbital_vel_over_lifetime->as_rotation = position_over_lifetime->as_rotation->val;
        }
        orbital_vel_over_lifetime->use_highp_float_on_ios = position_over_lifetime->useHighpFloatOnIOS;
        
        SizeOverLifeTimeData* size_over_lifetime = nullptr;
        if (props->size_over_lifetime) {
            size_over_lifetime = new SizeOverLifeTimeData;
            if (props->size_over_lifetime->separate_axes) {
                size_over_lifetime->separate_axes = props->size_over_lifetime->separate_axes->val;
            }
            if (size_over_lifetime->separate_axes) {
                if (props->size_over_lifetime->x) {
                    const auto& x = props->size_over_lifetime->x;
                    if (x->data_type_ == MDataType::ARRAY || x->number_ != 0) {
                        size_over_lifetime->size_x = VFXValueGetter::CreateValueGetter(x);
                    }
                }
                if (props->size_over_lifetime->y) {
                    const auto& y = props->size_over_lifetime->y;
                    if (y->data_type_ == MDataType::ARRAY || y->number_ != 0) {
                        size_over_lifetime->size_y = VFXValueGetter::CreateValueGetter(y);
                    }
                }
            } else {
                size_over_lifetime->size_x = VFXValueGetter::CreateValueGetter(props->size_over_lifetime->size);
                size_over_lifetime->size_y = VFXValueGetter::CreateValueGetter(props->size_over_lifetime->size);
            }
            
            if (!size_over_lifetime->size_x) {
                size_over_lifetime->size_x = VFXValueGetter::CreateConstantValueGetter(1);
            }
            if (!size_over_lifetime->size_y) {
                size_over_lifetime->size_y = VFXValueGetter::CreateConstantValueGetter(1);
            }
        }
        
        ColorOverLifeTimeData* color_over_lifetime = nullptr;
        if (props->color_over_lifetime) {
            color_over_lifetime = new ColorOverLifeTimeData;
            if (props->color_over_lifetime->opacity) {
                color_over_lifetime->opacity = VFXValueGetter::CreateValueGetter(props->color_over_lifetime->opacity);
            }
            if (props->color_over_lifetime->color) {
                VFXValueGetter::CreateGradientColor(color_over_lifetime->color_steps, props->color_over_lifetime->color->value_);
            }
        }
        
        int order = vfx_item->list_index_;
        std::string shader_cache_prefix = renderer_opt->cache_prefix;
        
        mesh_options_ = new ParticleMeshConstructor;
        mesh_options_->list_index = order;
        mesh_options_->name = vfx_item->name_;
        mesh_options_->matrix = new Mat4(render_matrix_);
        mesh_options_->shader_cache_prefix = shader_cache_prefix;
        mesh_options_->render_mode = renderer->render_mode ? new MInt(renderer->render_mode->val) : nullptr;
        mesh_options_->side = renderer->side ? new MInt(renderer->side->val) : nullptr;
        mesh_options_->gravity_modifier_ref = options_->gravity_modifier;
        mesh_options_->gravity = new Vec3();
        if (position_over_lifetime->gravity) {
            float* g = position_over_lifetime->gravity;
            mesh_options_->gravity->Set(g[0], g[1], g[2]);
        }
        mesh_options_->duration = vfx_item->duration_;
        mesh_options_->blending = renderer->blending ? new MInt(renderer->blending->val) : nullptr;
        mesh_options_->rotation_over_lifetime = rotation_over_lifetime;
        mesh_options_->linear_vel_over_lifetime = linear_vel_over_lifetime;
        mesh_options_->orbital_vel_over_lifetime = orbital_vel_over_lifetime;
        if (position_over_lifetime->speed_over_lifetime) {
            mesh_options_->speed_over_lifetime = VFXValueGetter::CreateValueGetter(position_over_lifetime->speed_over_lifetime);
        }
        mesh_options_->sprite_ref = texture_sheet_animation_;
        mesh_options_->occlusion = renderer->occlusion ? renderer->occlusion->val : false;
        mesh_options_->transparent_occlusion = renderer->transparent_occlusion ? new MBool(renderer->transparent_occlusion->val) : nullptr;
        mesh_options_->max_count = options->max_count ? options->max_count->val : false;
        mesh_options_->mask = renderer->mask ? new MInt(renderer->mask->val) : nullptr;
        mesh_options_->mask_mode = renderer->mask_mode ? new MInt(renderer->mask_mode->val) : nullptr;
        mesh_options_->force_target = force_target;
        mesh_options_->diffuse = InnerInitTexture(renderer->texture, renderer_opt);
        mesh_options_->size_over_lifetime = size_over_lifetime;
        mesh_options_->color_over_lifetime = color_over_lifetime;
        if (renderer->anchor) { // 有anchor使用anchor
            mesh_options_->anchor = new Vec2(*renderer->anchor);
        } else { // 没anchor使用particleOrigin
            mesh_options_->anchor = new Vec2;
            PARTICLE_ORIGIN_TYPE particle_origin = PARTICLE_ORIGIN_CENTER;
            if (renderer->particle_origin) {
                particle_origin = CreateParticleOriginType(renderer->particle_origin->val);
            }
            Translate::ConvertParticleOrigin2Anchor(mesh_options_->anchor, particle_origin);
        }
    }
    
    mesh_options_->texture_flip = InitTextureUV(props);
    particle_mesh_ = new ParticleMesh(mesh_options_, renderer_opt);
    
    transform_ = vfx_item->transform_;
    inner_transform_formula_ = new ParticleTransform;
    inner_transform_formula_->position_ = transform_->position_ ? new Vec3(*transform_->position_) : new Vec3();
    {
        Vec3 temp;
        transform_->GetRotation(temp);
        inner_transform_formula_->rotation_ = new Vec3(temp);
    }
    if (props->emitter_transform) {
        if (props->emitter_transform->path_arr) {
            float* p = props->emitter_transform->path_arr;
            inner_transform_formula_->position_->Add(Vec3(p[0], p[1], p[2]));
        } else {
            if (props->emitter_transform->path_value) {
                inner_transform_formula_->path_ = VFXValueGetter::CreateObjectValueGetter(props->emitter_transform->path_value);
            } else {
                inner_transform_formula_->path_ = VFXValueGetter::CreateConstantValueGetter(0);
            }
        }
    }
    
    UpdateEmitterTransform(0.0);
    meshes_.push_back(particle_mesh_->GetMesh());
    
    reusable_ = vfx_item->reusable_;
    SetHide(vfx_item->hide_);
}

ParticleSystem::~ParticleSystem() {
    MN_SAFE_DELETE(particle_mesh_);
    
    MN_SAFE_DELETE(options_);
    MN_SAFE_DELETE(emission_);
    MN_SAFE_DELETE(texture_sheet_animation_);
    
    MN_SAFE_DELETE(up_direction_world_);
    MN_SAFE_DELETE(inner_transform_formula_);
    
    MN_SAFE_DELETE(mesh_options_);
    MN_SAFE_DELETE(shape_);
}

void ParticleSystem::SetHide(bool hide) {
    particle_mesh_->GetMesh()->hide_ = hide;
    // trailMesh
}

void ParticleSystem::SetOptions(ParticleContentData* props, VFXItem* vfx_item) {
    auto options = props->options;
    auto position_over_lifetime = props->position_over_lifetime;
    auto shape = props->shape;
    auto duration = vfx_item->duration_;
    auto gravity_modifier = position_over_lifetime->gravity_modifier;
    
    bool start_turbulence = false;
    if (shape) {
        start_turbulence = (shape->turbulence_x || shape->turbulence_y || shape->turbulence_z);
    }
    
    options_ = new ParticleStartOptions;
    
    options_->start_life_time = VFXValueGetter::CreateValueGetter(options->start_lifetime);
    
    options_->start_delay = VFXValueGetter::CreateValueGetter(options->start_delay);
    
    options_->start_speed = VFXValueGetter::CreateValueGetter(position_over_lifetime->start_speed);
    
    if (options->start_color) {
        options_->start_color = VFXValueGetter::CreateColorValueGetter(options->start_color);
    }
    
    options_->end_behavior = vfx_item->end_behavior_;
    options_->duration = duration;
    options_->looping = (vfx_item->end_behavior_ == END_BEHAVIOR_RESTART);
    
    if (options->max_count) {
        options_->max_count = options->max_count->val;
    }
    
    if (gravity_modifier) {
        options_->gravity_modifier = VFXValueGetter::CreateValueGetter(gravity_modifier);
        options_->gravity_modifier->ScaleXCoord(duration);
    }
    
    if (options->start_3d_size) {
        options_->start_3D_size = options->start_3d_size->val;
    }
    
    options_->start_turbulence = start_turbulence;
    if (start_turbulence) {
        options_->turbulence.push_back(VFXValueGetter::CreateValueGetter(shape->turbulence_x));
        options_->turbulence.push_back(VFXValueGetter::CreateValueGetter(shape->turbulence_y));
        options_->turbulence.push_back(VFXValueGetter::CreateValueGetter(shape->turbulence_z));
    }
    
    if (options->start_rotation_z) {
        options_->start_rotation = VFXValueGetter::CreateValueGetter(options->start_rotation_z);
    }
    if (options->start_rotation_x || options->start_rotation_y) {
        options_->start_3D_rotation = true;
        options_->start_rotation_x = VFXValueGetter::CreateValueGetter(options->start_rotation_x);
        options_->start_rotation_y = VFXValueGetter::CreateValueGetter(options->start_rotation_y);
        options_->start_rotation_z = VFXValueGetter::CreateValueGetter(options->start_rotation_z);
    }
    
    if (options->start_3d_size && options->start_3d_size->val) {
        options_->start_size_x = VFXValueGetter::CreateValueGetter(options->start_size_x);
        options_->start_size_y = VFXValueGetter::CreateValueGetter(options->start_size_y);
    } else {
        options_->start_size = VFXValueGetter::CreateValueGetter(options->start_size);
        options_->size_aspect = VFXValueGetter::CreateValueGetter(options->size_aspect);
    }
}

void ParticleSystem::SetEmissionOption(ParticleContentData* props) {
    emission_ = new ParticleEmissionOptions;
    
    if (props->emission->rate_over_time) {
        emission_->rate_overtime = VFXValueGetter::CreateValueGetter(props->emission->rate_over_time);
    } else {
        emission_->rate_overtime = VFXValueGetter::CreateConstantValueGetter(0);
    }
    
    if (props->emission->bursts.size()) {
        const auto& bursts = props->emission->bursts;
        for (int i = 0; i < bursts.size() / 4; i++) {
            float time = bursts[i * 4];
            int count = bursts[i * 4 + 1];
            int cycles = bursts[i * 4 + 2];
            float interval = bursts[i * 4 + 3];
            
            std::shared_ptr<BurstOption> burst_option = std::make_shared<BurstOption>();
            
            burst_option->time = time;
            burst_option->interval = interval ? interval : 1;
            // todo: ValueGetter?
            burst_option->count = VFXValueGetter::CreateConstantValueGetter(count);
            burst_option->cycles = cycles;
            // todo: probability
            
            burst_option->Reset();
            emission_->bursts.push_back(burst_option);
        }
    }
    
    if (props->emission->burst_offsets.size()) {
        for (int i = 0; i < props->emission->burst_offsets.size() / 4; i++) {
            // todo: burstoffset前端有多种数据处理;
            int index = props->emission->burst_offsets[i * 4];
            float x = props->emission->burst_offsets[i * 4 + 1];
            float y = props->emission->burst_offsets[i * 4 + 2];
            float z = props->emission->burst_offsets[i * 4 + 3];
            
            Vec3 pos(x, y, z);
            emission_->SetBurstOffset(index, pos);
        }
    }
}

void ParticleSystem::SetShapeOption(ParticleContentData* props) {
    if (props->shape) {
        shape_ = new ParticleShapeOption;
        
        if (props->shape->shape) {
            shape_->shape = props->shape->shape;
        }
        
        if (props->shape->radius) {
            shape_->radius = props->shape->radius->val;
        }
        
        if (props->shape->arc) {
            shape_->arc = props->shape->arc->val;
        }
        
        if (props->shape->arc_mode) {
            shape_->SetArcMode(props->shape->arc_mode->val);
        }

        if (props->shape->width) {
            shape_->width = props->shape->width->val;
        }

        if (props->shape->height) {
            shape_->height = props->shape->height->val;
        }
        
        if (props->shape->align_speed_direction) {
            shape_->align_speed_direction = props->shape->align_speed_direction->val;
        }
        
        if (props->shape->up_direction) {
            float* u = props->shape->up_direction;
            shape_->up_direction.Set(u[0], u[1], u[2]);
        }
        
        if (props->shape->angle) {
            shape_->angle = props->shape->angle->val;
        }
        
        particle_shape_ = Shape::CreateShape(shape_);
    }
}

void ParticleSystem::SetTextureSheedAnimation(ParticleContentData* props) {
    auto texture = props->texture_sheet_animation;
    if (texture) {
        texture_sheet_animation_ = new TextureSheetAnimation;
        
        if (texture->animation_delay) {
            texture_sheet_animation_->animation_delay = VFXValueGetter::CreateValueGetter(texture->animation_delay);
        } else {
            texture_sheet_animation_->animation_delay = VFXValueGetter::CreateConstantValueGetter(0);
        }
        
        if (texture->animation_duration) {
            if (texture->animation_duration->data_type_ == MDataType::NUMBER) {
                float duration = texture->animation_duration->number_;
                if (duration == 0.f) {
                    duration = 1;
                }
                texture_sheet_animation_->animation_duration = VFXValueGetter::CreateConstantValueGetter(duration);
            } else {
                texture_sheet_animation_->animation_duration = VFXValueGetter::CreateValueGetter(texture->animation_duration);
            }
        } else {
            texture_sheet_animation_->animation_duration = VFXValueGetter::CreateConstantValueGetter(1.0f);
        }
        
        if (texture->cycles) {
            if (texture->cycles->data_type_ == MDataType::NUMBER) {
                float cycles = texture->cycles->number_;
                if (cycles == 0.f) {
                    cycles = 1;
                }
                texture_sheet_animation_->cycles = VFXValueGetter::CreateConstantValueGetter(cycles);
            } else {
                texture_sheet_animation_->cycles = VFXValueGetter::CreateValueGetter(texture->cycles);
            }
        } else {
            texture_sheet_animation_->cycles = VFXValueGetter::CreateConstantValueGetter(1.0f);
        }
        
        if (texture->animate) {
            texture_sheet_animation_->animate = texture->animate->val;
        }
        if (texture->col) {
            texture_sheet_animation_->col = texture->col->val;
        }
        if (texture->row) {
            texture_sheet_animation_->row = texture->row->val;
        }
        if (texture->total) {
            texture_sheet_animation_->total = texture->total->val;
        }
    }
}

void ParticleSystem::SetFilter() {
    // todo:
}

bool ParticleSystem::InitTextureUV(ParticleContentData* props) {
    std::vector<std::vector<float>> splits;
    bool has_splits = false;
    if (props->splits) {
        const auto& splits_data = props->splits;
        size_t count = splits_data->splits.size();
        for (int i = 0; i < count; i++) {
            const std::vector<float>& split = splits_data->splits[i];
            splits.push_back(split);
        }
        has_splits = true;
    }
    
    if (!has_splits) {
        std::vector<float> singleSplits {0.0, 0.0, 1.0, 1.0, 0.0};
        splits.push_back(singleSplits);
    }
    
    uvs_.clear();
    Vec4 texture_map(0, 0, 1, 1);
    auto split = splits[0];
    bool flip = split[4];
    if (flip) {
        texture_map = {split[0], split[1], split[3], split[2]};
    } else {
        texture_map = {split[0], split[1], split[2], split[3]};
    }
    
    if (texture_sheet_animation_ && !texture_sheet_animation_->animate) {
        int col = flip ? texture_sheet_animation_->row : texture_sheet_animation_->col;
        int row = flip ? texture_sheet_animation_->col : texture_sheet_animation_->row;
        int total;
        if (texture_sheet_animation_->total > 0) {
            total = texture_sheet_animation_->total;
        } else {
            total = col * row;
        }
        
        int index = 0;
        for (int x = 0; x < col; x++) {
            for (int y = 0; y < row; y++) {
                Vec4 uv;
                uv.m[0] = x * texture_map.m[2] / col + texture_map.m[0];
                uv.m[1] = y * texture_map.m[3] / row + texture_map.m[1];
                uv.m[2] = texture_map.m[2] / col;
                uv.m[3] = texture_map.m[3] / row;
                uvs_.push_back(uv);
                
                ++index;
                if (index >= total) {
                    break;
                }
            }
        }
    } else {
        uvs_.push_back(texture_map);
    }
    
    return flip;
}

void ParticleSystem::OnUpdate(float dt) {
    if (destroy_on_update_) {
        InnerOnDestroy();
        return;
    }
    // todo: independentEmitter;
    if (started_ && !freezed_) {
        float now = last_update_time_ + dt / 1000;
        last_update_time_ = now;
        float time_passed = this->TimePassed();
        particle_mesh_->SetTime(now);
        MN_SAFE_DELETE(up_direction_world_);
        
        // todo: with destroy and freeze; and looping;
        if (!ended_) {
            float duration = options_->duration;
            float lifetime = this->LifeTime();
            
            if (time_passed < duration) {
                float interval = 1 / emission_->rate_overtime->GetValue(lifetime);

                // todo: fix: result is nan sometime
                int point_count;
                double result = std::floor((time_passed - last_emit_time_) / interval);
                if (result >= std::numeric_limits<int>::min() && result <= std::numeric_limits<int>::max()) {
                    point_count = static_cast<int>(result);
                } else {
                    point_count = 0;
                }
                // point_count = (int) std::floor((time_passed - this->last_emit_time_) / interval);
            
                int max_emission_count = point_count;
                float time_delta = interval / point_count;
                float mesh_time = now;
                int max_count = options_->max_count;
                this->UpdateEmitterTransform(time_passed);
                
                for (size_t i=0; i<max_count && i<max_emission_count; i++) {
                    if (this->ShouldSkipGenerate()) {
                        break;
                    }
                    
                    std::unique_ptr<ParticlePoint> point = this->CreatePoint(lifetime);
                    point->delay += mesh_time + i * time_delta;
                    this->AddParticle(std::move(point), max_count);
                    this->last_emit_time_ = time_passed;
                }
                
                size_t burst_length = emission_->bursts.size();
                if (burst_length > 0) {
                    for (size_t j = 0, cursor = 0; j < burst_length && cursor < max_count; j++) {
                        if (this->ShouldSkipGenerate()) {
                            break;
                        }
                        
                        BurstGenerateOption burst_gen_option;
                        emission_->bursts[j]->GetGeneratorOption(burst_gen_option, time_passed, lifetime);
                        if (burst_gen_option.valid) {
                            Vec3 origin_vec;
                            Vec3 burst_offset;
                            this->emission_->GetBurstOffset(burst_offset, j, burst_gen_option.cycle_index);
                            
                            for (size_t i=0; i<burst_gen_option.count && cursor < max_count; i++) {
                                if (this->ShouldSkipGenerate()) {
                                    break;
                                }
                                
                                ShapeGeneratorOption shape_generator_option;
                                shape_generator_option.index = burst_gen_option.index;
                                shape_generator_option.total = burst_gen_option.total;
                                shape_generator_option.brust_index = i;
                                shape_generator_option.brust_count = burst_gen_option.count;
                                    
                                ShapeData shape_data;
                                if (this->particle_shape_) {
                                    this->particle_shape_->Generate(shape_generator_option, shape_data);
                                }
                                std::unique_ptr<ParticlePoint> point = this->InitPoint(shape_data);
                                cursor++;
                                point->delay += mesh_time;
                                point->pos.Add(burst_offset);
                                this->AddParticle(std::move(point), max_count);
                            }
                        }
                    }
                }
            } else if (options_->looping) {
                now -= duration;
                this->loop_start_time_ = now;
                this->last_emit_time_ -= duration;
                this->last_update_time_ -= duration;
                
                this->particle_mesh_->ReverseTime(duration);
                size_t link_length = this->particle_link_->Length();
                LinkNode* node = particle_link_->first_;
                for (size_t i = 0; i < link_length; i++) {
                    node->GetContext()[0] -= duration;
                    node->GetContext()[2] -= duration;
                    node = node->next_;
                }
            } else {
                this->ended_ = true;
//                this->OnEnd();
                if (END_BEHAVIOR_FREEZE == options_->end_behavior) {
                    this->freezed_ = true;
                }
                
            }
        } else if (!options_->looping) {
            // todo: reusable
            if (END_BEHAVIOR_DESTROY == options_->end_behavior) {
                auto& node = particle_link_->last_;
                if (node && (node->GetContext()[0] - loop_start_time_ < time_passed)) {
                    destroy_on_update_ = true;
                }
            }
        }
    }
}


void ParticleSystem::Reset() {
    this->particle_mesh_->ClearPoints();
    this->last_update_time_ = this->loop_start_time_ = 0;
    this->last_emit_time_ = -1 / this->emission_->rate_overtime->GetValue(0);
    this->particle_link_ = std::make_shared<LinkChain>();
    this->generated_count_ = 0;
    this->freezed_ = false;
}

void ParticleSystem::Start(ParticleSystemDestroyListener* destroy_listener) {
    destroy_listener_ = destroy_listener;
    if (!started_ || ended_) {
        this->Reset();
        this->started_ = true;
        this->ended_ = false;
    }
}

void ParticleSystem::Stop() {
    this->ended_ = true;
    this->started_ = false;
}

std::unique_ptr<ParticlePoint> ParticleSystem::CreatePoint(float life_time) {
    ShapeGeneratorOption shape_generator_option;
    shape_generator_option.index = this->generated_count_;
    shape_generator_option.total = this->emission_->rate_overtime->GetValue(life_time);
    this->generated_count_++;
    ShapeData shape_data;
    if (this->particle_shape_) {
        this->particle_shape_->Generate(shape_generator_option, shape_data);
    }
    return std::move(this->InitPoint(shape_data));
}

std::unique_ptr<ParticlePoint> ParticleSystem::InitPoint(const ShapeData& shape_data) {
    float lifetime = this->LifeTime();
    float speed = options_->start_speed->GetValue(lifetime);
    Mat4 matrix4;
    transform_->GetWorldMatrix(matrix4);
    Vec3 pos;
    pos.Set(shape_data.position);
    MathUtil::Vec3MulMat4(pos, pos, matrix4);
    Vec3 direction;
    direction.Set(shape_data.direction);
    MathUtil::Vec3RotateByMat4(direction, direction, matrix4);
    MathUtil::Vec3Normolize(direction);
    Vec3 temp_vec3;

    if (options_->start_turbulence) {
        Mat3 mat3;
        for (int i = 0; i < 3; i++) {
            temp_vec3.m[i] = options_->turbulence[i]->GetValue(lifetime);
        }
        MathUtil::Mat3FromRotation(mat3, temp_vec3);
        MathUtil::Vec3MulMat3(direction, direction, mat3);
        MathUtil::Vec3Normolize(direction);
    }
    
    Vec3 dir_x(1, 0, 0);
    Vec3 dir_y(0, 1, 0);
    if (shape_ && shape_->align_speed_direction) {
        dir_y.Set(direction);
        if (!up_direction_world_) {
            up_direction_world_ = new Vec3;
            MathUtil::Vec3RotateByMat4(*up_direction_world_, shape_->up_direction, matrix4);
        }
        MathUtil::Vec3Cross(dir_x, dir_y, *up_direction_world_);
        MathUtil::Vec3Normolize(dir_x);
    }
    
    Vec3 sprite;
    const auto tsa = texture_sheet_animation_;
    if (tsa && tsa->animate) {
        float delay = tsa->animation_delay->GetValue(lifetime);
        float duration = tsa->animation_duration->GetValue(lifetime);
        float cycle = tsa->cycles->GetValue(lifetime);
        sprite.Set(delay, duration, cycle);
    }
    
    Vec3 rot;
    if (options_->start_3D_rotation) {
        float x = options_->start_rotation_x->GetValue(lifetime);
        float y = options_->start_rotation_y->GetValue(lifetime);
        float z = options_->start_rotation_z->GetValue(lifetime);
        rot.Set(x, y, z);
    } else if (options_->start_rotation) {
        rot.Set(0, 0, options_->start_rotation->GetValue(lifetime));
    }
    
    Vec4 color;
    options_->start_color->GetValues(lifetime, color.m, color.Length());
    
    Vec2 size;
    if (options_->start_3D_size) {
        float x = options_->start_size_x->GetValue(lifetime);
        float y = options_->start_size_y->GetValue(lifetime);
        size.Set(x,y);
    } else {
        float n = options_->start_size->GetValue(lifetime);
        float aspect = options_->size_aspect->GetValue(lifetime);
        size.Set(n, n / aspect);
    }
    
    this->transform_->GetWorldTRS(nullptr, nullptr, &temp_vec3);
    Vec3 tem_size(size.m[0], size.m[1], 1);
    MathUtil::Vec3MulCombine(tem_size, &tem_size, &temp_vec3);
    size.m[0] = tem_size.m[0];
    size.m[1] = tem_size.m[1];
    
    Vec4 uv;
    MathUtil::RandomArrItem(uv, this->uvs_);
    
    // todo: refactor;
    std::unique_ptr<ParticlePoint> point = std::make_unique<ParticlePoint>();
    MathUtil::Vec3Dot(point->vel, direction, speed);
    point->color.Set(color);
    point->pos.Set(pos);
    point->delay = options_->start_delay->GetValue(lifetime);
    point->lifetime = options_->start_life_time->GetValue(lifetime);
    point->size.Set(size);
    point->uv.Set(uv);
    point->sprite.Set(sprite);
    point->rot.Set(rot);
    point->dir_x.Set(dir_x);
    point->dir_y.Set(dir_y);
    return std::move(point); // todo: warning
}

void ParticleSystem::AddParticle(std::unique_ptr<ParticlePoint> point, size_t max_count) {
    auto link = this->particle_link_;
    std::vector<float> line_context({point->delay + point->lifetime, 0, point->delay, point->size.m[0]});
    size_t cursor = 0;
    if (link->Length() < max_count) {
        cursor = link->Length();
        line_context[1] = (float) cursor;
    } else {
        LinkNode* first = link->first_;
        line_context[1] = (first->GetContext())[1];
        cursor = (size_t)line_context[1];
        link->RemoveNode(first);
    }

    link->PushNode(line_context);
    this->particle_mesh_->SetPoint(std::move(point), cursor);
}

// todo: particle transform;
void ParticleSystem::UpdateEmitterTransform(float time) {
    auto transform = inner_transform_formula_;
    auto node = transform_;
    
    bool changed = false;
    if (parent_transform_ || transform->path_) {
        changed = true;
    }
    
    if (changed) {
        Vec3 self_pos;
        this->transform_->Invalid();
        if (transform->position_) {
            self_pos.Add(*inner_transform_formula_->position_);
        }
        
        if (transform->path_) {
            Vec3 temp;
            float duration = options_->duration;
            float life_time = time / duration;
            transform->path_->GetValues(life_time, temp.m, temp.Length());
            self_pos.Add(temp);
        }
        node->SetPosition(self_pos);
    }
}

bool ParticleSystem::ShouldSkipGenerate() {
    if (emission_stopped_) {
        return true;
    }
    
    // link_chain的长度满了，同时头部的时间最短的元素，还未超出timePassed的时候,暂停生成Point；
    if (particle_link_) {
        size_t link_len = particle_link_->Length();
        if (link_len == options_->max_count) {
            LinkNode *first =  particle_link_->first_;
            if (first) {
                float node_life = first->GetContext()[0];
                if ((node_life - loop_start_time_) > this->TimePassed()) {
                    return true;
                }
            }
        }
    }
    
    return false;
}


void ParticleSystem::Raycast() {
    
}

void ParticleSystem::GetParentMatrix() {
    
}

std::shared_ptr<MarsTexture> ParticleSystem::InnerInitTexture(MInt* texture, MeshRendererOptions* opt) {
    if (texture) {
        return opt->composition->GetTextureContainerById(texture->val);
    }
    auto tex = opt->empty_texture;
    tex->AssignRenderer(opt->renderer);
    return tex;
}

void ParticleSystem::InnerOnDestroy() {
    destroy_listener_->DestroyParticleItem(this);
}

}
