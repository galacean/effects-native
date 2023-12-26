//
//  common_properties_decoder.cpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/6/4.
//

#include "common_properties_decoder.hpp"
#include "sceneData/decoder/filter_content_decoder.hpp"

namespace mn {

ItemRendererOptionsData* CommonPropertiesDecoder::DecodeRendererOptions(MBuffer* buffer) {
    PRINT_DATA_STRING("rendererOptions", "begin--------------------------");

    auto renderer = new ItemRendererOptionsData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ItemRendererOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = renderer;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_int) {
                renderer->render_mode = new MInt(buffer->ReadInt());
                PRINT_DATA_INT("renderer rendeMode", renderer->render_mode_->val);
            } else catch_data_exception(renderMode);
            break;

        case 2:
            if (data_type_is_int) {
                renderer->blending = new MInt(buffer->ReadInt());
                PRINT_DATA_INT("renderer blending_", renderer->blending_->val);
            } else catch_data_exception(blending_);
            break;

        case 3:
            if (data_type_is_int) {
                renderer->side = new MInt(buffer->ReadInt());
                PRINT_DATA_INT("renderer side_", renderer->side_->val);
            } else catch_data_exception(side_);
            break;

        case 4:
            if (data_type_is_int) {
                renderer->occlusion = new MBool(buffer->ReadInt() == 1);
                PRINT_DATA_INT("renderer occlusion_", renderer->occlusion_->val);
            } else catch_data_exception(occlusion);
            break;

        case 5:
            if (data_type_is_int) {
                renderer->transparent_occlusion = new MBool(buffer->ReadInt() == 1);
                PRINT_DATA_INT("renderer transparent_occlusion_", renderer->transparent_occlusion_->val);
            } else catch_data_exception(transparent_occlusion_);
            break;

        case 6:
            if (data_type_is_int) {
                renderer->texture = new MInt(buffer->ReadInt());
                PRINT_DATA_INT("renderer texture_", renderer->texture_->val);
            } else catch_data_exception(texture);
            break;

        case 7:
            if (data_type_is_int) {
                renderer->mask_mode = new MInt(buffer->ReadInt());
                PRINT_DATA_INT("renderer mask_mode_", renderer->mask_mode_->val);
            } else catch_data_exception(mask_mode_);
            break;

        case 8:
            if (data_type_is_int) {
                renderer->particle_origin = new MInt(buffer->ReadInt());
                PRINT_DATA_INT("renderer particle_origin_", renderer->particle_origin_->val);
            } else catch_data_exception(particle_origin_);
            break;

        case 9:
            if (data_type_is_int) {
                // todo: ? MarsStudio后续版本会去掉这个参数
                renderer->order = new MFloat(buffer->ReadInt());
                PRINT_DATA_INT("renderer order_", renderer->order_->val);
            } else catch_data_exception(order_);
            break;

        case 10:
            if (data_type_is_int) {
                renderer->shape_id = new MInt(buffer->ReadInt());
                PRINT_DATA_INT("renderer shape_id_", renderer->shape_id_->val);
            } else catch_data_exception(shape_id_);
            break;
                
        case 11:
            if (data_type_is_floatarray) {
                renderer->anchor = new Vec2;
                float* tmp = buffer->ReadFloatArray(2);
                renderer->anchor->Set(tmp[0], tmp[1]);
            } else catch_data_exception(anchor);
            break;

        default:
            MLOGE("sprite content options invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("rendererOptions", "end--------------------------");
    holder.ptr_ = nullptr;
    return renderer;
}

ItemSizeOverLifetimeData* CommonPropertiesDecoder::DecodeSizeOLT(MBuffer* buffer) {
    PRINT_DATA_STRING("spriteContentSizeOLT", "begin--------------------------");

    ItemSizeOverLifetimeData* size = new ItemSizeOverLifetimeData();
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ItemSizeOverLifetimeData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = size;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_int) {
                size->separate_axes = new MBool(buffer->ReadInt() == 1);
                PRINT_DATA_INT("size separateAxes", size->separate_axes_->val);
            } else catch_data_exception(separateAxes);
            break;

        case 2:
            if (data_type_is_numbder_exp) {
                size->size = DecodeNumberExpression(buffer);
                if (!size->size) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("size size", size->size_);
            } else catch_data_exception(size);
            break;

        case 3:
            if (data_type_is_numbder_exp) {
                size->x = DecodeNumberExpression(buffer);
                if (!size->x) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("size x", size->x_);
            } else catch_data_exception(x);
            break;

        case 4:
            if (data_type_is_numbder_exp) {
                size->y = DecodeNumberExpression(buffer);
                if (!size->y) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("size y", size->y_);
            } else catch_data_exception(y);
            break;

        case 5:
            if (data_type_is_numbder_exp) {
                size->z = DecodeNumberExpression(buffer);
                if (!size->z) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("size z", size->z_);
            } else catch_data_exception(z);
            break;

        default:
            MLOGE("sprite content sizeolt invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("spriteContentSizeOLT", "end--------------------------");
    holder.ptr_ = nullptr;
    return size;
}

ItemRotateOverLifetimeData* CommonPropertiesDecoder::DecodeRotateOLT(MBuffer* buffer) {
    PRINT_DATA_STRING("spriteContentRotateOLT", "begin--------------------------");

    ItemRotateOverLifetimeData* rotate = new ItemRotateOverLifetimeData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ItemRotateOverLifetimeData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = rotate;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_int) {
                rotate->separate_axes = new MBool(buffer->ReadInt() == 1);
                PRINT_DATA_INT("rotate separateAxes", rotate->separate_axes_->val);
            } else catch_data_exception(separateAxes);
            break;

        case 2:
            if (data_type_is_int) {
                rotate->as_rotation = new MBool(buffer->ReadInt() == 1);
                PRINT_DATA_INT("rotate as_rotation_", rotate->as_rotation_->val);
            } else catch_data_exception(as_rotation_);
            break;

        case 3:
            if (data_type_is_numbder_exp) {
                rotate->x = DecodeNumberExpression(buffer);
                if (!rotate->x) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("rotate x", rotate->x_);
            } else catch_data_exception(x);
            break;

        case 4:
            if (data_type_is_numbder_exp) {
                rotate->y = DecodeNumberExpression(buffer);
                if (!rotate->y) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("rotate y", rotate->y_);
            } else catch_data_exception(y);
            break;

        case 5:
            if (data_type_is_numbder_exp) {
                rotate->z = DecodeNumberExpression(buffer);
                if (!rotate->z) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("rotate z", rotate->z_);
            } else catch_data_exception(z);
            break;

        default:
            MLOGE("sprite content rotateolt invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("spriteContentRotateOLT", "end--------------------------");
    holder.ptr_ = nullptr;
    return rotate;
}

ItemColorOverLifetimeData* CommonPropertiesDecoder::DecodeColorOLT(MBuffer* buffer) {
    PRINT_DATA_STRING("spriteContentColorOLT", "begin--------------------------");

    ItemColorOverLifetimeData* color = new ItemColorOverLifetimeData();
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ItemColorOverLifetimeData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = color;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_color_exp) {
                int type = buffer->ReadInt();
                if (type == 9) {
                    auto v = std::make_shared<MValue>();
                    v->type_ = "gradient";
                    v->length_ = buffer->ReadInt() * 5;
                    v->data_ = buffer->ReadFloatArray(v->length_);
                    color->color = new MNumberOrValue(v);
                } else {
                    MLOGE("invalid color type %d", type);
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("color color", color->color_);
            } else catch_data_exception(color);
            break;

        case 2:
            if (data_type_is_numbder_exp) {
                color->opacity = DecodeNumberExpression(buffer);
                if (!color->opacity) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("color opacity", color->opacity_);
            } else catch_data_exception(opacity)
            break;

        default:
            MLOGE("sprite content colorolt invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("spriteContentColorOLT", "end--------------------------");
    holder.ptr_ = nullptr;
    return color;
}

ItemPositionOverLifetimeData* CommonPropertiesDecoder::DecodePositionOLT(MBuffer* buffer) {
    PRINT_DATA_STRING("spriteContentPositionOLT", "begin--------------------------");

    ItemPositionOverLifetimeData* position = new ItemPositionOverLifetimeData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ItemPositionOverLifetimeData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = position;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_int) {
                position->as_movement = new MBool(buffer->ReadInt());
                PRINT_DATA_INT("position asMovement", position->as_movement_->val);
            } else catch_data_exception(asMovement);
            break;

        case 2:
            if (data_type_is_numbder_exp) {
                position->linear_x = DecodeNumberExpression(buffer);
                if (!position->linear_x) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("position licearX", position->linear_x_);
            } else catch_data_exception(linearX);
            break;

        case 3:
            if (data_type_is_numbder_exp) {
                position->linear_y = DecodeNumberExpression(buffer);
                if (!position->linear_y) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("position licearY", position->linear_y_);
            } else catch_data_exception(linearY);
            break;

        case 4:
            if (data_type_is_numbder_exp) {
                position->linear_z = DecodeNumberExpression(buffer);
                if (!position->linear_z) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("position licearZ", position->linear_z_);
            } else catch_data_exception(linearZ);
            break;

        case 5:
            if (data_type_is_int) {
                position->as_rotation = new MBool(buffer->ReadInt());
                PRINT_DATA_INT("position asRotation", position->as_rotation_->val);
            } else catch_data_exception(asRotation);
            break;

        case 6:
            if (data_type_is_numbder_exp) {
                position->orbital_x = DecodeNumberExpression(buffer);
                if (!position->orbital_x) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("position orbitalX", position->orbital_x_);
            } else catch_data_exception(orbital_x_);
            break;

        case 7:
            if (data_type_is_numbder_exp) {
                position->orbital_y = DecodeNumberExpression(buffer);
                if (!position->orbital_y) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("position orbitalY", position->orbital_y_);
            } else catch_data_exception(orbital_y_);
            break;

        case 8:
            if (data_type_is_numbder_exp) {
                position->orbital_z = DecodeNumberExpression(buffer);
                if (!position->orbital_z) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("position orbitalZ", position->orbital_z_);
            } else catch_data_exception(orbital_z_);
            break;

        case 9:
            if (data_type_is_floatarray) {
                position->orbital_center = buffer->ReadFloatArray(3);
                PRINT_DATA_FLOAT_ARRAY("position orbitalCenter", position->orbital_center_, 3);
            } else catch_data_exception(orbitalCenter);
            break;

        case 10:
            if (data_type_is_vec3_exp) {
                if (!DecodeVec3Expression(position, buffer)) {
                    return nullptr;
                }
                if (position->path_arr) {
                    PRINT_DATA_FLOAT_ARRAY("position path", node->transform_->path_arr_, 3);
                } else {
                    PRINT_DATA_VALUE("position path", node->transform_->path_value_);
                }
            } else catch_data_exception(path);
            break;

        case 11:
            if (data_type_is_floatarray) {
                position->direction = buffer->ReadFloatArray(3);
                PRINT_DATA_FLOAT_ARRAY("position direction", node->options_->direction_, 3);
            } else catch_data_exception(direction);
            break;

        case 12:
            if (data_type_is_float) {
                position->start_speed = new MNumberOrValue(buffer->ReadFloat());
            } else if (data_type_is_numbder_exp) {
                position->start_speed = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!position->start_speed) {
                    return nullptr;
                }
            } else catch_data_exception(startSpeed);
            PRINT_DATA_NUMBER_EXP("position startSpeed", node->options_->start_speed_);
            break;

        case 13:
            if (data_type_is_numbder_exp) {
                position->speed_over_lifetime = DecodeNumberExpression(buffer);
                if (!position->speed_over_lifetime) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("position speedOLT", position->speed_over_lifetime_);
            } else catch_data_exception(speedOverLifetime);
            break;

        case 14:
            if (data_type_is_floatarray) {
                position->gravity = buffer->ReadFloatArray(3);
                PRINT_DATA_FLOAT_ARRAY("position gravity", node->options_->gravity_, 3);
            } else catch_data_exception(gravity);
            break;

        case 15:
            if (data_type_is_numbder_exp) {
                position->gravity_modifier = DecodeNumberExpression(buffer);
                if (!position->gravity_modifier) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("position gravity_modifier_", node->options_->gravity_modifier_);
            } else catch_data_exception(gravityOverLifetime);
            break;

        case 16:
            if (data_type_is_int) {
                position->force_target = new MBool(buffer->ReadInt() == 1);
                PRINT_DATA_INT("position forceTarget", position->force_target_->val);
            } else catch_data_exception(forceTarget);
            break;

        case 17:
            if (data_type_is_floatarray) {
                position->target = buffer->ReadFloatArray(3);
                PRINT_DATA_FLOAT_ARRAY("position target", position->target_, 3);
            } else catch_data_exception(target);
            break;

        case 18:
            if (data_type_is_numbder_exp) {
                position->force_curve = DecodeNumberExpression(buffer);
                PRINT_DATA_VALUE("position force_curve_", position->force_curve_);
            } else catch_data_exception(forceCurve);
            break;
                
        case 19:
            if (data_type_is_int) {
                position->useHighpFloatOnIOS = buffer->ReadInt()  == 1;
                MLOGD("useHighpFloatOnIOS %d", position->useHighpFloatOnIOS);
            } else catch_data_exception(useHighpFloatOnIOS);
            break;

        default:
            MLOGE("sprite content positionolt invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("spriteContentPositionOLT", "end--------------------------");
    holder.ptr_ = nullptr;
    return position;
}

ItemTextureSheetAnimationData* CommonPropertiesDecoder::DecodeTextureSheetAni(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeTextureShaeetAni", "begin--------------------------");

    ItemTextureSheetAnimationData* texture = new ItemTextureSheetAnimationData();
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ItemTextureSheetAnimationData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = texture;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_int) {
                texture->row = new MInt(buffer->ReadInt());
            } else catch_data_exception(row);
            break;

        case 2:
            if (data_type_is_int) {
                texture->col = new MInt(buffer->ReadInt());
            } else catch_data_exception(col_);
            break;

        case 3:
            if (data_type_is_int) {
                texture->total = new MInt(buffer->ReadInt());
            } else catch_data_exception(total_);
            break;

        case 4:
            if (data_type_is_int) {
                texture->animate = new MBool(buffer->ReadInt() == 1);
            } else catch_data_exception(animate_);
            break;

        case 5:
            if (data_type_is_numbder_exp) {
                texture->cycles = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!texture->cycles) {
                    return nullptr;
                }
            } else catch_data_exception(cycles_);
            break;

        case 6:
            if (data_type_is_numbder_exp) {
                texture->animation_delay = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!texture->animation_delay) {
                    return nullptr;
                }
            } else catch_data_exception(animation_delay_);
            break;

        case 7:
            if (data_type_is_numbder_exp) {
                texture->animation_duration = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!texture->animation_duration) {
                    return nullptr;
                }
            } else catch_data_exception(animation_duration_);
            break;

        default:
            MLOGE("sprite content rotateolt invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("DecodeTextureShaeetAni", "end--------------------------");
    holder.ptr_ = nullptr;
    return texture;
}


ItemFilterData* CommonPropertiesDecoder::DecodeFilterOption(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeFilterOptions", "begin--------------------------");
    
    DataTag tag;
    tag.val = buffer->ReadInt();
    
    std::string name = buffer->ReadString();
    PRINT_DATA_STRING("filter name", filter->name.c_str());
    
    if (name == "gaussian") {
        return FilterContentDecoder::GaussionFilterContentDecode(buffer);
    } else if (name == "delay") {
        return FilterContentDecoder::DelayFilterContentDecode(buffer);
    } else if (name == "bloom") {
        return FilterContentDecoder::BloomFilterContentDecode(buffer);
    } else if (name == "distortion") {
        return FilterContentDecoder::DistortionFilterContentDecode(buffer);
    } else {
        return nullptr;
    }
}


MNumberOrValue* CommonPropertiesDecoder::DecodeNumberExpression(MBuffer* buffer) {
    int type = buffer->ReadInt();

    switch (type) {
    case 0:
        return new MNumberOrValue(buffer->ReadFloat());
    
    case 4:
    {
        std::shared_ptr<MValue> v = std::make_shared<MValue>();
        v->type_ = "random";
        v->length_ = 2;
        v->data_ = buffer->ReadFloatArray(2);
        return new MNumberOrValue(v);
    }

    case 5:
    {
        std::shared_ptr<MValue> v = std::make_shared<MValue>();
        int length = buffer->ReadInt();
        v->type_ = "lines";
        v->length_ = 2 * length;
        v->data_ = buffer->ReadFloatArray(2 * length);
        return new MNumberOrValue(v);
    }

    case 6:
    {
        std::shared_ptr<MValue> v = std::make_shared<MValue>();
        int length = buffer->ReadInt();
        v->type_ = "curve";
        v->length_ = 4 * length;
        v->data_ = buffer->ReadFloatArray(4 * length);
        return new MNumberOrValue(v);
    }
    
    default:
        break;
    }

    MLOGE("DecodeNumberExpression invalid type %d", type);
    return nullptr;
}

bool CommonPropertiesDecoder::DecodeVec3Expression(ParticleContentEmitterTransformData* position, MBuffer* buffer) {
    int type = buffer->ReadInt();

    switch (type) {
    case 2:
        position->path_arr = buffer->ReadFloatArray(3);
        return true;
            
    case 7:
    {
        auto v = new MValue();
        int length = buffer->ReadInt();
        v->type_ = "bezier";
        v->length_ = 7 * length;
        v->data_ = buffer->ReadFloatArray(7 * length);
        v->length2_ = 3 * buffer->ReadInt();
        v->data2_ = buffer->ReadFloatArray(v->length2_);
        position->path_value = v;
        return true;
    }

    case 12:
    {
        auto v = new MValue();
        int length = buffer->ReadInt();
        v->type_ = "path";
        v->length_ = 7 * length;
        v->data_ = buffer->ReadFloatArray(7 * length);
        position->path_value = v;
        return true;
    }
    
    default:
        break;
    }

    MLOGE("DecodeVec3Expression invalid type %d", type);
    return false;
}

}
