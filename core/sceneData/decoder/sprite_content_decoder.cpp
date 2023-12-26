#include "sprite_content_decoder.h"
#include "common_properties_decoder.hpp"

namespace mn {

static SpriteContentOptionsData* DecodeSpriteContentOptions(MBuffer* buffer) {
    PRINT_DATA_STRING("spriteContentOptions", "begin--------------------------");
    
    SpriteContentOptionsData* options = new SpriteContentOptionsData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        SpriteContentOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = options;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_floatarray) {
                options->start_color = new MValue;
                options->start_color->type_ = "color";
                options->start_color->length_ = 4;
                options->start_color->data_ = buffer->ReadFloatArray(4);
                PRINT_DATA_VALUE("item options startColor", node->options_->start_color_);
            } else catch_data_exception(startColor);
            break;

        default:
            MLOGE("sprite content options invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("spriteContentOptions", "end--------------------------");
    holder.ptr_ = nullptr;
    return options;
}

static void* DecodeSpriteInteraction(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeSpriteInteraction", "begin--------------------------");

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_int) {
                buffer->ReadInt();
            } else catch_data_exception(behavior);
            break;

        default:
            MLOGE("sprite content interaction invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("DecodeSpriteInteraction", "end--------------------------");
    return nullptr;
}

SpriteContentData* SpriteContentDecoder::Decode(MBuffer* buffer) {
    PRINT_DATA_STRING("spriteContent", "begin--------------------------");

    SpriteContentData* node = new SpriteContentData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        SpriteContentData* ptr_ = nullptr;
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
                node->options = DecodeSpriteContentOptions(buffer);
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
                node->size_over_lifetime = CommonPropertiesDecoder::DecodeSizeOLT(buffer);
                if (!node->size_over_lifetime) {
                    return nullptr;
                }
            } else catch_data_exception(sizeOverLifetime);
            break;

        case 5:
            if (data_type_is_object) {
                node->rotation_over_lifetime = CommonPropertiesDecoder::DecodeRotateOLT(buffer);
                if (!node->rotation_over_lifetime) {
                    return nullptr;
                }
            } else catch_data_exception(rotateOverLifetime_);
            break;

        case 6:
            if (data_type_is_object) {
                node->position_over_lifetime = CommonPropertiesDecoder::DecodePositionOLT(buffer);
                if (!node->position_over_lifetime) {
                    return nullptr;
                }
            } else catch_data_exception(postionOVerLifetime)
            break;

        case 7:
            if (data_type_is_object) {
                node->color_over_lifetime = CommonPropertiesDecoder::DecodeColorOLT(buffer);
                if (!node->color_over_lifetime) {
                    return nullptr;
                }
            } else catch_data_exception(colorOverLifetime);
            break;
                
        case 8:
            if (data_type_is_object) {
                node->texture_sheet_animation = CommonPropertiesDecoder::DecodeTextureSheetAni(buffer);
                if (!node->texture_sheet_animation) {
                    return nullptr;
                }
            } else catch_data_exception(textureSheetAnimation);
            break;

        case 9:
            if (data_type_is_object) {
                // 暂时没用
                DecodeSpriteInteraction(buffer);
            } else catch_data_exception(interaction);
            break;
                
        case 10:
            if (data_type_is_object) {
                node->filter_data = CommonPropertiesDecoder::DecodeFilterOption(buffer);
                if (!node->filter_data) {
                    return nullptr;
                }
            } else catch_data_exception(filterOption);
            break;
                
        default:
            MLOGE("sprite content invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("spriteContent", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

}
