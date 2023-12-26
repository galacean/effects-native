//
//  composition_decoder.cpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/6/3.
//

#include "composition_decoder.hpp"
#include "item_decoder.hpp"

namespace mn {

CameraData* DecodeCamera(MBuffer* buffer) {
    CameraData* camera = new CameraData();
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        CameraData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = camera;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_float) {
                camera->fov = buffer->ReadFloat();
                PRINT_DATA_FLOAT("camera fov", camera->fov_);
            } else catch_data_exception(fov);
            break;

        case 2:
            if (data_type_is_float) {
                camera->far = buffer->ReadFloat();
                PRINT_DATA_FLOAT("camera far", camera->far_);
            } else catch_data_exception(far_);
            break;

        case 3:
            if (data_type_is_float) {
                camera->near = buffer->ReadFloat();
                PRINT_DATA_FLOAT("camera near", camera->near_);
            } else catch_data_exception(near_);
            break;

        case 4:
            if (data_type_is_floatarray) {
                camera->position = buffer->ReadFloatArray(3);
                PRINT_DATA_FLOAT_ARRAY("camera position_", camera->position_, 3);
            } else catch_data_exception(position_);
            break;

        case 5:
            if (data_type_is_floatarray) {
                camera->rotation = buffer->ReadFloatArray(3);
                PRINT_DATA_FLOAT_ARRAY("camera rotation_", camera->rotation_, 3);
            } else catch_data_exception(rotation_);
            break;

        case 6:
            if (data_type_is_int) {
                camera->clip_mode = new MInt(buffer->ReadInt());
                PRINT_DATA_INT("camera clip_mode_", camera->clip_mode_->val);
            } else catch_data_exception(clip_mode_);
            break;
        
        default:
            MLOGE("camera invalid tag %d", tag.val);
            return nullptr;
            break;
        }
        
        tag.val = buffer->ReadInt();
    }

    holder.ptr_ = nullptr;
    return camera;
}

CompositionData* CompositionDecoder::Decode(MBuffer* buffer) {
    PRINT_DATA_STRING("comp", "begin--------------------------");

    auto comp = new CompositionData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(comp_);
        }
        
        CompositionData* comp_ = nullptr;
    };
    Holder holder;
    holder.comp_ = comp;
    
    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_string) {
                comp->id = buffer->ReadString();
                PRINT_DATA_STRING("compId", comp->id_.c_str());
            } else catch_data_exception(compId);
            break;

        case 2:
            if (data_type_is_string) {
                comp->name = buffer->ReadString();
                PRINT_DATA_STRING("compName", comp->name_);
            } else catch_data_exception(compName);
            break;

        case 3:
            if (data_type_is_float) {
                comp->duration = buffer->ReadFloat();
                PRINT_DATA_FLOAT("comp duration", comp->duration_);
            } else catch_data_exception(compDuration);
            break;

        case 4:
            if (data_type_is_int) {
                comp->end_behavior = new MInt(buffer->ReadInt());
                PRINT_DATA_INT("comp endBehavior", comp->end_behavior_->val);
            } else catch_data_exception(compEndBeh);
            break;

        case 5:
            if (data_type_is_object) {
                comp->camera = DecodeCamera(buffer);
                if (!comp->camera) {
                    return nullptr;
                }
            } else catch_data_exception(compCamera);
            break;
                
        case 6:
            if (data_type_is_object) {
                int item_count = buffer->ReadInt();
                PRINT_DATA_INT("items count", item_count);
                for (int i = 0; i < item_count; i++) {
                    auto item = ItemDecoder::Decode(buffer);
                    if (!item) {
                        return nullptr;
                    }
                    if (!item->type) {
                        MLOGE("CompDecoder: item type is undefine");
                        return nullptr;
                    }
                    comp->items.push_back(item);
                }
            } else catch_data_exception(compItems);
            break;

        case 7:
            if (data_type_is_floatarray) {
                comp->preview_size = buffer->ReadFloatArray(2);
                PRINT_DATA_FLOAT_ARRAY("comp previewSize", comp->preview_size, 2);
            } else catch_data_exception(previewSize);
            break;
        
        default:
            MLOGE("comp invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }
    
    PRINT_DATA_STRING("comp", "end--------------------------");
    holder.comp_ = nullptr;
    return comp;
}

}

