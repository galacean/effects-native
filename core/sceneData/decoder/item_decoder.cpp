//
//  item_decoder.cpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/6/3.
//

#include "item_decoder.hpp"
#include "sprite_content_decoder.h"
#include "particle_content_decoder.hpp"
#include "interact_content_decoder.h"
#include "plugin_content_decoder.h"
#include "util/constant.hpp"
#include "model/sceneData/decoder/model_content_decoder.h"

namespace mn {

static ItemTransformData* DecodeTransform(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeTransform", "begin--------------------------");
    
    ItemTransformData* transform = new ItemTransformData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ItemTransformData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = transform;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_floatarray) {
                transform->position = buffer->ReadFloatArray(3);
                PRINT_DATA_FLOAT_ARRAY("transform position", node->transform_->position_, 3);
            } else catch_data_exception(position);
            break;

        case 2:
            if (data_type_is_floatarray) {
                transform->rotation = buffer->ReadFloatArray(3);
                PRINT_DATA_FLOAT_ARRAY("transform rotation", node->transform_->rotation_, 3);
            } else catch_data_exception(rotation);
            break;
                
        case 3:
            if (data_type_is_floatarray) {
                transform->scale = buffer->ReadFloatArray(3);
                PRINT_DATA_FLOAT_ARRAY("transform scale", temp, 3);
            } else catch_data_exception(scale);
            break;
        
        default:
            MLOGE("item invalid tag %d", tag.val);
            return nullptr;
            break;
        }
        
        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("DecodeTransform", "end--------------------------");
    holder.ptr_ = nullptr;
    return transform;
}

ItemData* ItemDecoder::Decode(MBuffer* buffer) {
    PRINT_DATA_STRING("item", "begin--------------------------");
    
    auto item = new ItemData();
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ItemData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = item;
    
    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_string) {
                item->id = buffer->ReadString();
                PRINT_DATA_STRING("item id", item->id_.c_str());
            } else catch_data_exception(id);
            break;

        case 2:
            if (data_type_is_string) {
                item->name = buffer->ReadString();
                PRINT_DATA_STRING("item name_", item->name_);
            } else catch_data_exception(name_);
            break;

        case 3:
            if (data_type_is_float) {
                item->duration = buffer->ReadFloat();
                PRINT_DATA_FLOAT("item duration_", duration->val);
            } else catch_data_exception(duration_);
            break;

        case 4:
            if (data_type_is_string) {
                auto type_str = buffer->ReadString();
                int type = 0;
                if (strcmp(type_str, "1") == 0) {
                    type = ITEM_CONTENT_TYPE_SPRITE;
                } else if (strcmp(type_str, "2") == 0) {
                    type = ITEM_CONTENT_TYPE_PARTICLE;
                } else if (strcmp(type_str, "3") == 0) {
                    type = ITEM_CONTENT_TYPE_CALCULATE;
                } else if (strcmp(type_str, "4") == 0) {
                    type = ITEM_CONTENT_TYPE_INTERACT;
                } else if (strcmp(type_str, "5") == 0) {
                    type = ITEM_CONTENT_TYPE_PLUGIN;
                } else if (strcmp(type_str, "8") == 0) {
                    type = ITEM_CONTENT_TYPE_FILTER;
                } else if (strcmp(type_str, "mesh") == 0) {
                    type = ITEM_CONTENT_TYPE_MODEL_MESH;
                } else if (strcmp(type_str, "tree") == 0) {
                    type = ITEM_CONTENT_TYPE_MODEL_TREE;
                } else if (strcmp(type_str, "skybox") == 0) {
                    type = ITEM_CONTENT_TYPE_MODEL_SKYBOX;
                } else if (strcmp(type_str, "light") == 0) {
                    type = ITEM_CONTENT_TYPE_MODEL_LIGHT;
                } else {
                    MLOGE("invalid type %s", type_str);
                    return nullptr;
                }
                item->type = type;
                PRINT_DATA_INT("item type", type);
            } else catch_data_exception(type);
            break;

        case 5:
            if (data_type_is_string) {
                item->parent_id = buffer->ReadString();
                PRINT_DATA_STRING("item parentId", item->parent_id_);
            } else catch_data_exception(parentId)
            break;
        
        case 6:
            if (data_type_is_int) {
                item->visible = new MBool(buffer->ReadInt() == 1);
                PRINT_DATA_INT("item visible", visible);
            } else catch_data_exception(visible)
            break;

        case 7:
            if (data_type_is_int) {
                item->end_behavior = new MInt(buffer->ReadInt());
                PRINT_DATA_INT("item endBehavior", GetItemOptions(item)->end_behavior_->val);
            } else catch_data_exception(endBehavior);
            break;

        case 8:
            if (data_type_is_float) {
                item->delay = buffer->ReadFloat();
                PRINT_DATA_FLOAT("item delay", item->delay_);
            } else catch_data_exception(delay)
            break;

        case 9:
            if (data_type_is_object) {
                if (item->type == ITEM_CONTENT_TYPE_SPRITE || item->type == ITEM_CONTENT_TYPE_CALCULATE) {
                    item->content = SpriteContentDecoder::Decode(buffer);
                } else if (item->type == ITEM_CONTENT_TYPE_PARTICLE) {
                    item->content = ParticleContentDecoder::Decode(buffer);
                } else if (item->type == ITEM_CONTENT_TYPE_INTERACT) {
                    item->content = InteractContentDecoder::Decode(buffer);
                } else if (item->type == ITEM_CONTENT_TYPE_FILTER) {
                    item->content = SpriteContentDecoder::Decode(buffer);
                } else if (item->type == ITEM_CONTENT_TYPE_PLUGIN) {
                    item->content = PluginContentDecoder::Decode(buffer);
                } else if (item->type == ITEM_CONTENT_TYPE_MODEL_MESH) {
                    MLOGD("decode model mesh length: %d", buffer->ReadInt());
                    item->content = ModelContentDecoder::DecodeMeshContent(buffer);
                } else if (item->type == ITEM_CONTENT_TYPE_MODEL_TREE) {
                    MLOGD("decode model tree length: %d", buffer->ReadInt());
                    item->content = ModelContentDecoder::DecodeTreeContent(buffer);
                } else if (item->type == ITEM_CONTENT_TYPE_MODEL_SKYBOX) {
                    MLOGD("decode model skybox length: %d", buffer->ReadInt());
                    item->content = ModelContentDecoder::DecodeSkyboxContent(buffer);
                } else if (item->type == ITEM_CONTENT_TYPE_MODEL_LIGHT) {
                    MLOGD("decode model light length: %d", buffer->ReadInt());
                    item->content = ModelContentDecoder::DecodeLightContent(buffer);
                } else {
                    MLOGE("item invalid content type %d", item->type);
                    return nullptr;
                }
                if (!item->content) {
                    MLOGE("item content is null");
                    return nullptr;
                }
            } else catch_data_exception(content);
            break;

        case 10:
            if (data_type_is_string) {
                item->render_level = CreateRenderLevel(buffer->ReadString());
            } else catch_data_exception(renderLevel);
            break;

        case 11:
            if (data_type_is_int) {
                // 暂时没用
                buffer->ReadInt();
            } else catch_data_exception(pn);
            break;

        case 12:
            if (data_type_is_string) {
                // 暂时没用
                buffer->ReadString();
            } else catch_data_exception(pluginName);
            break;

        case 13:
            if (data_type_is_object) {
                item->transform = DecodeTransform(buffer);
                if (!item->transform) {
                    return nullptr;
                }
            } else catch_data_exception(transform);
            break;
        
        default:
            MLOGE("item invalid tag %d", tag.val);
            return nullptr;
            break;
        }
        
        tag.val = buffer->ReadInt();
    }
    
    PRINT_DATA_STRING("item", "end--------------------------");
    holder.ptr_ = nullptr;
    return item;
}

}
