//
//  particle_content_decoder.cpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/6/5.
//

#include "particle_content_decoder.hpp"
#include "common_properties_decoder.hpp"

namespace mn {

static ParticleContentOptionsData* DecodeOptions(MBuffer* buffer) {
    PRINT_DATA_STRING("particleContentOptions", "begin--------------------------");

    auto options = new ParticleContentOptionsData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ParticleContentOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = options;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_int) {
                options->max_count = new MInt(buffer->ReadInt());
                PRINT_DATA_INT("options maxCount", options->max_count_->val);
            } else catch_data_exception(maxCount);
            break;

        case 2:
            if (data_type_is_numbder_exp) {
                options->start_lifetime = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                PRINT_DATA_NUMBER_EXP("options startLifetime", options->start_lifetime_);
            } else catch_data_exception(startLifetime);
            break;

        case 3:
            if (data_type_is_numbder_exp) {
                options->start_delay = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                PRINT_DATA_NUMBER_EXP("options start_delay_", options->start_delay_);
            } else catch_data_exception(start_delay_);
            break;

        case 4:
            if (data_type_is_color_exp) {
                int type = buffer->ReadInt();
                options->start_color = new MValue();
                if (type == 8) {
                    options->start_color->type_ = "color";
                    options->start_color->length_ = 4;
                    options->start_color->data_ = buffer->ReadFloatArray(4);
                } else if (type == 9) {
                    options->start_color->type_ = "gradient";
                    options->start_color->length_ = buffer->ReadInt() * 5;
                    options->start_color->data_ = buffer->ReadFloatArray(options->start_color->length_);
                } else if (type == 13) {
                    options->start_color->type_ = "colors";
                    options->start_color->length_ = buffer->ReadInt() * 4;
                    options->start_color->data_ = buffer->ReadFloatArray(options->start_color->length_);
                } else {
                    MLOGE("invalid color type %d", type);
                    return nullptr;
                }
                PRINT_DATA_VALUE("options start_color_", options->start_color_);
            } else catch_data_exception(startColor);
            break;

        case 5:
            if (data_type_is_numbder_exp) {
                options->start_size = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!options->start_size) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("options startSize", options->start_size_);
            } else catch_data_exception(start_size_);
            break;

        case 6:
            if (data_type_is_numbder_exp) {
                options->size_aspect = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!options->size_aspect) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("options sizeAspect", options->size_aspect_);
            } else catch_data_exception(size_aspect_);
            break;

        case 7:
            if (data_type_is_int) {
                options->start_3d_size = new MBool(buffer->ReadInt());
                PRINT_DATA_INT("options start3dSize", options->start_3d_size_->val);
            } else catch_data_exception(start3DSize);
            break;

        case 8:
            if (data_type_is_numbder_exp) {
                options->start_size_x = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!options->start_size_x) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("options start_size_x_", options->start_size_x_);
            } else catch_data_exception(start_size_x_);
            break;

        case 9:
            if (data_type_is_numbder_exp) {
                options->start_size_y = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!options->start_size_y) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("options start_size_y_", options->start_size_y_);
            } else catch_data_exception(start_size_y_);
            break;

        case 10:
            if (data_type_is_numbder_exp) {
                options->start_rotation_z = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!options->start_rotation_z) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("options start_rotation_z_", options->start_rotation_z_);
            } else catch_data_exception(start_rotation_z_);
            break;

        case 11:
            if (data_type_is_numbder_exp) {
                options->start_rotation_x = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!options->start_rotation_x) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("options start_rotation_x_", options->start_rotation_x_);
            } else catch_data_exception(start_rotation_x_);
            break;

        case 12:
            if (data_type_is_numbder_exp) {
                options->start_rotation_y = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!options->start_rotation_y) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("options start_rotation_y_", options->start_rotation_y_);
            } else catch_data_exception(start_rotation_y_);
            break;

        default:
            MLOGE("sprite content options invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("particleContentOptions", "end--------------------------");
    holder.ptr_ = nullptr;
    return options;
}

static ParticleContentShapeData* DecodeShape(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeShape", "begin--------------------------");

    auto shape = new ParticleContentShapeData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ParticleContentShapeData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = shape;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_int) {
                int type = buffer->ReadInt();
                switch (type) {
                case 0:
                    shape->shape = "None";
                    break;
                case 1:
                    shape->shape = "Sphere";
                    break;
                case 2:
                    shape->shape = "Cone";
                    break;
                case 3:
                    shape->shape = "Hemisphere";
                    break;
                case 4:
                    shape->shape = "Circle";
                    break;
                case 5:
                    shape->shape = "Donut";
                    break;
                case 6:
                    shape->shape = "Rectangle";
                    break;
                case 7:
                    shape->shape = "RectangleEdge";
                    break;
                case 8:
                    shape->shape = "Edge";
                    break;
                default:
                    MLOGE("invalid shape type %d", type);
                    return nullptr;
                }
                PRINT_DATA_STRING("shape shape", shape->shape_);
                PRINT_DATA_INT("shape shapeId", type);
            } else catch_data_exception(type);
            break;

        case 2:
            if (data_type_is_float) {
                shape->angle = new MFloat(buffer->ReadFloat());
                PRINT_DATA_FLOAT("shape angle", shape->angle_->val);
            } else catch_data_exception(angle);
            break;

        case 3:
            if (data_type_is_float) {
                shape->radius = new MFloat(buffer->ReadFloat());
                PRINT_DATA_FLOAT("shape radius", shape->radius_->val);
            } else catch_data_exception(radius);
            break;

        case 4:
            if (data_type_is_float) {
                shape->radius = new MFloat(buffer->ReadFloat());
                PRINT_DATA_FLOAT("shape donutRadius", shape->radius_->val);
            } else catch_data_exception(donutradius);
            break;

        case 5:
            if (data_type_is_float) {
                shape->width = new MFloat(buffer->ReadFloat());
                PRINT_DATA_FLOAT("shape width", shape->width_->val);
            } else catch_data_exception(width_);
            break;

        case 6:
            if (data_type_is_float) {
                shape->height = new MFloat(buffer->ReadFloat());
                PRINT_DATA_FLOAT("shape height", shape->height_->val);
            } else catch_data_exception(height_);
            break;

        case 7:
            if (data_type_is_numbder_exp) {
                shape->turbulence_x = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!shape->turbulence_x) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("shape turbulenceX", node->options_->turbulence_x_);
            } else catch_data_exception(turbulenceX)
            break;

        case 8:
            if (data_type_is_numbder_exp) {
                shape->turbulence_y = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!shape->turbulence_y) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("shape turbulenceY", node->options_->turbulence_y_);
            } else catch_data_exception(turbulenceY)
            break;

        case 9:
            if (data_type_is_numbder_exp) {
                shape->turbulence_z = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!shape->turbulence_z) {
                    return nullptr;
                }
                PRINT_DATA_NUMBER_EXP("shape turbulenceZ", node->options_->turbulence_z_);
            } else catch_data_exception(turbulenceZ)
            break;

        case 10:
            if (data_type_is_float) {
                shape->arc = new MFloat(buffer->ReadFloat());
                PRINT_DATA_FLOAT("shape arc", shape->arc_->val);
            } else catch_data_exception(arc);
            break;

        case 11:
            if (data_type_is_int) {
                shape->arc_mode = new MInt(buffer->ReadInt());
                PRINT_DATA_INT("shape arcMode", shape->arc_mode_->val);
            } else catch_data_exception(arc_mode_);
            break;

        case 12:
            if (data_type_is_int) {
                shape->align_speed_direction = new MBool(buffer->ReadInt() == 1);
                PRINT_DATA_INT("shape align_speed_direction_", shape->align_speed_direction_->val);
            } else catch_data_exception(align_speed_direction_);
            break;

        case 13:
            if (data_type_is_floatarray) {
                shape->up_direction = buffer->ReadFloatArray(3);
                PRINT_DATA_FLOAT_ARRAY("shape upDirection", shape->up_direction_, 3);
            } else catch_data_exception(upDirection)
            break;

        case 14:
            // todo：不支持random
            return nullptr;
            break;

        default:
            MLOGE("particle content shape invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("DecodeShape", "end--------------------------");
    holder.ptr_ = nullptr;
    return shape;
}

static ParticleContentEmissionData* DecodeEmission(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeEmission", "begin--------------------------");

    auto emission = new ParticleContentEmissionData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ParticleContentEmissionData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = emission;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_numbder_exp) {
                emission->rate_over_time = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                PRINT_DATA_NUMBER_EXP("emossion rateOverTime", emission->rate_over_time_);
            } else catch_data_exception(rateOLT);
            break;

        case 2:
            if (data_type_is_object) {
                int length = buffer->ReadInt();
                for (int i = 0; i < length; i++) {
                    emission->bursts.push_back(buffer->ReadFloat());
                    emission->bursts.push_back(buffer->ReadFloat());
                    emission->bursts.push_back(buffer->ReadFloat());
                    emission->bursts.push_back(buffer->ReadFloat());
                }
            } else catch_data_exception(bursts)
            break;

        case 3:
            if (data_type_is_object) {
                int length = buffer->ReadInt();
                for (int i = 0; i < length; i++) {
                    emission->burst_offsets.push_back(buffer->ReadInt());
                    emission->burst_offsets.push_back(buffer->ReadFloat());
                    emission->burst_offsets.push_back(buffer->ReadFloat());
                    emission->burst_offsets.push_back(buffer->ReadFloat());
                }
            } else catch_data_exception(bursts)
            break;

        default:
            MLOGE("particle emission invalid tag %d", tag.val);
            return nullptr;
            break;
        }
        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("DecodeEmission", "end--------------------------");
    holder.ptr_ = nullptr;
    return emission;
}

static ItemSizeOverLifetimeData* DecodeSizeOLT(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeSizeOLT", "begin--------------------------");

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
            } else catch_data_exception(separateAxes);
            break;

        case 2:
            if (data_type_is_numbder_exp) {
                size->size = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!size->size) {
                    return nullptr;
                }
            } else catch_data_exception(size);
            break;

        case 3:
            if (data_type_is_numbder_exp) {
                size->x = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!size->x) {
                    return nullptr;
                }
            } else catch_data_exception(x_);
            break;

        case 4:
            if (data_type_is_numbder_exp) {
                size->y = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                if (!size->y) {
                    return nullptr;
                }
            } else catch_data_exception(y_);
            break;

        default:
            MLOGE("particle size invalid tag %d", tag.val);
            return nullptr;
            break;
        }
        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("DecodeSizeOLT", "end--------------------------");
    holder.ptr_ = nullptr;
    return size;
}

static ParticleContentEmitterTransformData* DecodeEmissionTransform(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeEmissionTransform", "begin--------------------------");
    
    ParticleContentEmitterTransformData* et = new ParticleContentEmitterTransformData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ParticleContentEmitterTransformData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = et;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_vec3_exp) {
                if (!CommonPropertiesDecoder::DecodeVec3Expression(et, buffer)) {
                    return nullptr;
                }
            } else catch_data_exception(separateAxes);
            break;

        default:
            MLOGE("particle emissionTransform invalid tag %d", tag.val);
            return nullptr;
            break;
        }
        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("DecodeEmissionTransform", "end--------------------------");
    holder.ptr_ = nullptr;
    return et;
}

ParticleContentData* ParticleContentDecoder::Decode(MBuffer* buffer) {
    PRINT_DATA_STRING("particleContent", "begin--------------------------");

    ParticleContentData* node = new ParticleContentData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ParticleContentData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_object) {
                int length = buffer->ReadInt();
                if (length != 0) {
                    node->splits = new ItemSplitsData();
                    for (int i = 0; i < length; i++) {
                        std::vector<float> arr;
                        for (int j = 0; j < 5; j++) {
                            arr.push_back(buffer->ReadFloat());
                        }
                        node->splits->splits.push_back(arr);
                    }
                }
            } else catch_data_exception(splits);
            break;

        case 2:
            if (data_type_is_object) {
                node->options = DecodeOptions(buffer);
                if (!node->options) {
                    return nullptr;
                }
            } else catch_data_exception(options);
            break;

        case 3:
            if (data_type_is_object) {
                node->renderer = CommonPropertiesDecoder::DecodeRendererOptions(buffer);
                if (!node->renderer) {
                    return nullptr;
                }
            } else catch_data_exception(renderer);
            break;

        case 4:
            if (data_type_is_object) {
                node->shape = DecodeShape(buffer);
                if (!node->shape) {
                    return nullptr;
                }
            } else catch_data_exception(shape);
            break;

        case 5:
            if (data_type_is_object) {
                node->emission = DecodeEmission(buffer);
                if (!node->emission) {
                    return nullptr;
                }
            } else catch_data_exception(emission);
            break;

        case 6:
            if (data_type_is_object) {
                node->size_over_lifetime = DecodeSizeOLT(buffer);
                if (!node->size_over_lifetime) {
                    return nullptr;
                }
            } else catch_data_exception(size);
            break;
                
        case 7:
            if (data_type_is_object) {
                node->emitter_transform = DecodeEmissionTransform(buffer);
                if (!node->emitter_transform) {
                    return nullptr;
                }
            } else catch_data_exception(emissionTransform);
            break;

        case 8:
            if (data_type_is_object) {
                node->position_over_lifetime = CommonPropertiesDecoder::DecodePositionOLT(buffer);
                if (!node->position_over_lifetime) {
                    return nullptr;
                }
            } else catch_data_exception(positionOverLifetime);
            break;

        case 9:
            if (data_type_is_object) {
                node->rotation_over_lifetime = CommonPropertiesDecoder::DecodeRotateOLT(buffer);
                if (!node->rotation_over_lifetime) {
                    return nullptr;
                }
            } else catch_data_exception(rotateOverLifetime_);
            break;

        case 10:
            if (data_type_is_object) {
                node->color_over_lifetime = CommonPropertiesDecoder::DecodeColorOLT(buffer);
                if (!node->color_over_lifetime) {
                    return nullptr;
                }
            } else catch_data_exception(colorOverLifetime_);
            break;

        case 11:
            if (data_type_is_object) {
                node->texture_sheet_animation = CommonPropertiesDecoder::DecodeTextureSheetAni(buffer);
                if (!node->texture_sheet_animation) {
                    return nullptr;
                }
            } else catch_data_exception(texture_sheet_animation_);
            break;
        
        default:
            MLOGE("particle content invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("particleContent", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

}
