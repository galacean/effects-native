//
//  model_content_decoder.cpp
//
//  Created by changxing on 2022/10/13.
//

#include "model_content_decoder.h"
#include "util/log_util.hpp"

namespace mn {

static uint8_t* DecodeBinaryData(MBuffer* buffer, int& length, int* type) {
    int bin_idx = buffer->ReadInt();
    int start_offset = buffer->ReadInt();
    int byte_length = buffer->ReadInt();
    int data_type = buffer->ReadInt();
    if (type) {
        *type = data_type;
    }
    length = byte_length;
    if (bin_idx < 0 || bin_idx >= buffer->bins_.size()) {
        return nullptr;
    }
    const auto& bin = buffer->bins_[bin_idx];
    if (byte_length == -1) {
        byte_length = bin.length - start_offset;
    }
    if (byte_length <= 0) {
        return nullptr;
    }
    length = byte_length;
    return bin.data + start_offset;
}

static GLTFTextureData* DecodeGLTFTexture(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeGLTFTexture", "begin--------------------------");
    GLTFTextureData* data = new GLTFTextureData;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
            data->mag_filter = new MInt(buffer->ReadInt());
            break;
            
        case 2:
            data->min_filter = new MInt(buffer->ReadInt());
            break;
    
        case 3:
            data->wrap_s = new MInt(buffer->ReadInt());
            break;
    
        case 4:
            data->wrap_t = new MInt(buffer->ReadInt());
            break;
    
        case 5:
            data->source = new MInt(buffer->ReadInt());
            break;
    
        case 6:
            data->image_file_id = new MInt(buffer->ReadInt());
            break;
            
//        case 7:
//            data->flip_y = (buffer->ReadUint32() != 0);
//            break;
                
            case 8:
            {
                uint32_t temp_target = buffer->ReadUint32();
                if (temp_target == 34067) {
                    data->cube_mipmaps = new GLTFCubeTextureMipmapData;
                    int length = buffer->ReadInt();
                    for (int i = 0; i < length * 6; i++) {
                        BinaryData* bin = new BinaryData;
                        bin->data_ref = DecodeBinaryData(buffer, bin->length, nullptr);
                        data->cube_mipmaps->datas.push_back(bin);
                    }
                } else {
                    delete data;
                    MLOGE("DecodeGLTFTexture invalid mipmap target: %u", temp_target);
                    return nullptr;
                }
            }
                break;
            case 9:
                data->target = new MInt(buffer->ReadInt());
                break;
            case 10:
                data->source_type = new MInt(buffer->ReadInt());
                break;
            case 11:
                data->format = new MInt(buffer->ReadInt());
                break;
            case 12:
                data->interal_format = new MInt(buffer->ReadInt());
                break;
            case 13:
                data->type = new MInt(buffer->ReadInt());
                break;
        default:
            delete data;
            MLOGE("DecodeGLTFTexture invalid key: %u", key);
            return nullptr;
        }
        key = buffer->ReadUint32();
    }
    
    PRINT_DATA_STRING("DecodeGLTFTexture", "end--------------------------");
    return data;
}

static std::shared_ptr<MarsTextureTransform> DecodeTextureTransform(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeTransform", "begin--------------------------");
    std::shared_ptr<MarsTextureTransform> data = std::make_shared<MarsTextureTransform>();
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
        {
            int length = buffer->ReadInt();
            if (length != 2) {
                MLOGE("DecodeTransform offset invalid length: %d", length);
                return nullptr;
            }
            data->offset.Set(buffer->ReadFloat(), buffer->ReadFloat());
        }
            break;
            
        case 2:
            data->rotation = buffer->ReadFloat();
            break;
            
        case 3:
        {
            int length = buffer->ReadInt();
            if (length != 2) {
                MLOGE("DecodeTransform scale invalid length: %d", length);
                return nullptr;
            }
            data->scale.Set(buffer->ReadFloat(), buffer->ReadFloat());
        }
            break;
            
        default:
            MLOGE("DecodeTransform invalid key: %u", key);
            return nullptr;
        }
        key = buffer->ReadUint32();
    }
    
    PRINT_DATA_STRING("DecodeTransform", "begin--------------------------");
    return data;
}

static ItemTransformData* DecodeItemTransform(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeItemTransform", "begin--------------------------");
    ItemTransformData* ret = new ItemTransformData;
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
            {
                int length = buffer->ReadInt();
                if (length != 3) {
                    delete ret;
                    MLOGE("DecodeItemTransform invalid position length");
                    return nullptr;
                }
                ret->position = buffer->ReadFloatArray(length);
            }
            break;
            
        case 2:
            {
                int length = buffer->ReadInt();
                if (length != 3) {
                    delete ret;
                    MLOGE("DecodeItemTransform invalid rotation length");
                    return nullptr;
                }
                ret->rotation = buffer->ReadFloatArray(length);
            }
            break;
            
        case 3:
            {
                int length = buffer->ReadInt();
                if (length != 3) {
                    delete ret;
                    MLOGE("DecodeItemTransform invalid scale length");
                    return nullptr;
                }
                ret->scale = buffer->ReadFloatArray(length);
            }
            break;
                
        case 4:
        {
            int length = buffer->ReadInt();
            if (length != 4) {
                delete ret;
                MLOGE("DecodeItemTransform invalid quat length");
                return nullptr;
            }
            ret->quat = buffer->ReadFloatArray(length);
        }
            break;
            
        default:
            delete ret;
            MLOGE("DecodeItemTransform invalid key %u", key);
            return nullptr;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("DecodeItemTransform", "end--------------------------");
    return ret;
}

static SkinOptionsData* DecodeModelMeshOptionsSkin(MBuffer* buffer) {
    PRINT_DATA_STRING("modelMeshContentOptionsSkin", "begin--------------------------");
    SkinOptionsData* node = new SkinOptionsData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        SkinOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
            node->name = buffer->ReadString();
            break;
                
        case 2:
            node->joints_length = buffer->ReadInt();
            node->joints = buffer->ReadIntArray(node->joints_length);
            break;
                
        case 3:
            node->skeleton = new MInt(buffer->ReadInt());
            break;
                
        case 4:
        {
            int type = 0;
            node->inverse_bind_matrices = (float*) DecodeBinaryData(buffer, node->inverse_bind_matrices_length, &type);
            if (!node->inverse_bind_matrices) {
                MLOGE("decode skin inverse_bind_matrices fail");
                return nullptr;
            }
//            if (type != MODEL_TYPE_Float32Array) {
//                MLOGE("decode skin inverse_bind_matrices invalid type");
//                return nullptr;
//            }
        }
            break;
                
        default:
            MLOGE("model mesh options skin invalid tag %u", key);
            return nullptr;
            break;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("modelMeshContentOptionsSkin", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

static MaterialOptionsData* DecodeModelMeshOptionsPrimitiveMaterial(MBuffer* buffer) {
    PRINT_DATA_STRING("modelMeshOptionsPrimitiveMaterial", "begin--------------------------");
    
    uint32_t key = buffer->ReadUint32();
    if (key != 1) {
        MLOGE("DecodeModelMeshOptionsPrimitiveMaterial first key is not type");
        return nullptr;
    }
    std::string type = buffer->ReadString();
    MaterialOptionsData* node;
    if (type.compare("unlit") == 0) {
        node = new MaterialUnlitOptionsData;
    } else if (type.compare("pbr") == 0) {
        node = new MaterialPBROptionsData;
    } else {
        MLOGE("invalid material type %s", type.c_str());
        return nullptr;
    }
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        MaterialOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 2:
            node->name = buffer->ReadString();
            break;
                
        case 3:
            node->blending = new MUint(buffer->ReadUint32());
            break;
                
        case 4:
            node->side = new MUint(buffer->ReadUint32());
            break;
                
        case 5:
            node->alpha_cut_off = new MFloat(buffer->ReadFloat());
            break;
                
        case 6:
        {
            int length = buffer->ReadInt();
            if (length != 4) {
                MLOGE("invalid baseColorFactor length");
                return nullptr;
            }
            node->base_color_factor = buffer->ReadFloatArray(4);
        }
            break;
                
        case 7:
            node->base_color_texture = DecodeGLTFTexture(buffer);
            if (!node->base_color_texture) {
                return nullptr;
            }
            break;
                
        case 8:
            node->base_color_texture_transform = std::shared_ptr<MarsTextureTransform>(DecodeTextureTransform(buffer));
            if (!node->base_color_texture_transform) {
                return nullptr;
            }
            break;
                
        case 9:
            node->base_color_texture_coordinate = new MFloat(buffer->ReadFloat());
            break;
                
        case 10:
            ((MaterialPBROptionsData*) node)->metallic_factor = buffer->ReadFloat();
            break;
                
        case 11:
            ((MaterialPBROptionsData*) node)->roughness_factor = buffer->ReadFloat();
            break;
                
        case 12:
            ((MaterialPBROptionsData*) node)->metallic_roughness_texture = DecodeGLTFTexture(buffer);
            if (!((MaterialPBROptionsData*) node)->metallic_roughness_texture) {
                return nullptr;
            }
            break;
                
        case 13:
            ((MaterialPBROptionsData*) node)->metallic_roughness_texture_transform = std::shared_ptr<MarsTextureTransform>(DecodeTextureTransform(buffer));
            if (!((MaterialPBROptionsData*) node)->metallic_roughness_texture_transform) {
                return nullptr;
            }
            break;
            
        case 14:
            ((MaterialPBROptionsData*) node)->metallic_roughness_texture_coordinate = new MFloat(buffer->ReadFloat());
            break;
            
        case 15:
            ((MaterialPBROptionsData*) node)->normal_texture = DecodeGLTFTexture(buffer);
            if (!((MaterialPBROptionsData*) node)->normal_texture) {
                return nullptr;
            }
            break;
            
        case 16:
            ((MaterialPBROptionsData*) node)->normal_texture_scale = new MFloat(buffer->ReadFloat());
            break;
            
        case 17:
            ((MaterialPBROptionsData*) node)->normal_texture_transform = std::shared_ptr<MarsTextureTransform>(DecodeTextureTransform(buffer));
            if (!((MaterialPBROptionsData*) node)->normal_texture_transform) {
                return nullptr;
            }
            break;
            
        case 18:
            ((MaterialPBROptionsData*) node)->normal_texture_coordinate = new MFloat(buffer->ReadFloat());
            break;
            
        case 19:
            ((MaterialPBROptionsData*) node)->occlusion_texture = DecodeGLTFTexture(buffer);
            if (!((MaterialPBROptionsData*) node)->occlusion_texture) {
                return nullptr;
            }
            break;
            
        case 20:
            ((MaterialPBROptionsData*) node)->occlusion_texture_strength = new MFloat(buffer->ReadFloat());
            break;
            
        case 21:
            ((MaterialPBROptionsData*) node)->occlusion_texture_transform = std::shared_ptr<MarsTextureTransform>(DecodeTextureTransform(buffer));
            if (!((MaterialPBROptionsData*) node)->occlusion_texture_transform) {
                return nullptr;
            }
            break;
            
        case 22:
            ((MaterialPBROptionsData*) node)->occlusion_texture_coordinate = new MFloat(buffer->ReadFloat());
            break;
            
        case 23:
        {
            int length = buffer->ReadInt();
            if (length != 4) {
                MLOGE("invalid emissiveFactor length");
                return nullptr;
            }
            ((MaterialPBROptionsData*) node)->emissive_factor.Set(buffer->ReadFloat(), buffer->ReadFloat(), buffer->ReadFloat(), buffer->ReadFloat());
        }
            break;
            
        case 24:
            ((MaterialPBROptionsData*) node)->emissive_intensity = buffer->ReadFloat();
            break;
            
        case 25:
            ((MaterialPBROptionsData*) node)->emissive_texture = DecodeGLTFTexture(buffer);
            if (!((MaterialPBROptionsData*) node)->emissive_texture) {
                return nullptr;
            }
            break;
            
        case 26:
            ((MaterialPBROptionsData*) node)->emissive_texture_transform = std::shared_ptr<MarsTextureTransform>(DecodeTextureTransform(buffer));
            if (!((MaterialPBROptionsData*) node)->emissive_texture_transform) {
                return nullptr;
            }
            break;
            
        case 27:
            ((MaterialPBROptionsData*) node)->emissive_texture_coordinate = new MFloat(buffer->ReadFloat());
            break;
            
        case 28:
            ((MaterialPBROptionsData*) node)->enable_shadow = new MBool(buffer->ReadUint32() > 0);
            break;
                
        default:
            MLOGE("model mesh content options primitive invalid tag %u", key);
            return nullptr;
            break;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("modelMeshOptionsPrimitiveMaterial", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

static GLTFBufferAttributeData* DecodeGLTFBufferAttributeData(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeModelGeometryData", "begin--------------------------");
    GLTFBufferAttributeData* node = new GLTFBufferAttributeData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        GLTFBufferAttributeData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
        {
            node->array = DecodeBinaryData(buffer, node->array_length, nullptr);
            if (!node->array) {
                MLOGE("decode bufferAttr data fail");
                return nullptr;
            }
        }
            break;
                
        case 2:
            node->type = buffer->ReadUint32();
            break;
                
        case 3:
            node->item_size = buffer->ReadInt();
            break;
            
        case 4:
            node->stride = buffer->ReadInt();
            break;
            
        case 5:
            node->offset = buffer->ReadInt();
            break;
            
        case 6:
            node->normalize = (buffer->ReadUint32() != 0);
            break;
            
        default:
            MLOGE("DecodeGLTFBufferAttributeData invalid tag %u", key);
            return nullptr;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("DecodeModelGeometryData", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

static ModelGeometryData* DecodeModelGeometryData(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeModelGeometryData", "begin--------------------------");
    ModelGeometryData* node = new ModelGeometryData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ModelGeometryData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
            node->mode = buffer->ReadUint32();
            break;
                
        case 2:
            node->draw_start = buffer->ReadInt();
            break;
                
        case 3:
            node->draw_count = new MInt(buffer->ReadInt());
            break;
            
        case 4:
        {
            int length = 0;
            int type = 0;
            uint8_t* data = DecodeBinaryData(buffer, length, &type);
            if (!data) {
                MLOGE("DecodeModelGeometryData invalid indices");
                return nullptr;
            }
            node->indices = new GLTFBufferAttributeData;
            node->indices->type = type;
            node->indices->array = data;
            node->indices->array_length = length;
        }
            break;
            
        case 5:
        {
            int length = buffer->ReadInt();
            for (int i = 0; i < length; i++) {
                std::string name = buffer->ReadString();
                auto data = DecodeGLTFBufferAttributeData(buffer);
                if (!data) {
                    MLOGE("DecodeModelGeometryData invalid attr");
                    return nullptr;
                }
                node->attributes[name] = data;
            }
        }
            break;
                
        case 6:
            node->instance_count = new MInt(buffer->ReadInt());
            break;
            
        case 7:
            node->buffer_usage = new MUint(buffer->ReadUint32());
            break;
                
        default:
            MLOGE("DecodeModelGeometryData invalid tag %u", key);
            return nullptr;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("DecodeModelGeometryData", "begin--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

static PrimitiveOptionsData* DecodeModelMeshOptionsPrimitiveObj(MBuffer* buffer) {
    PRINT_DATA_STRING("modelMeshContentOptionsPrimitive", "begin--------------------------");
    PrimitiveOptionsData* node = new PrimitiveOptionsData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        PrimitiveOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
            node->geometry = DecodeModelGeometryData(buffer);
            if (!node->geometry) {
                MLOGE("model mesh content options primitive geometry fail");
                return nullptr;
            }
            break;
                
        case 2:
            node->material = DecodeModelMeshOptionsPrimitiveMaterial(buffer);
            if (!node->material) {
                MLOGE("model mesh content options primitive material fail");
                return nullptr;
            }
            break;
                
        default:
            MLOGE("model mesh content options primitive invalid tag %u", key);
            return nullptr;
            break;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("modelMeshContentOptionsPrimitive", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

static ModelMeshOptionsData* DecodeModelMeshOptions(MBuffer* buffer) {
    PRINT_DATA_STRING("modelMeshContentOptions", "begin--------------------------");
    ModelMeshOptionsData* node = new ModelMeshOptionsData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ModelMeshOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
            node->skin = DecodeModelMeshOptionsSkin(buffer);
            if (!node->skin) {
                MLOGE("model mesh options decode skin fail");
                return nullptr;
            }
            break;
                
        case 2:
        {
            int length = buffer->ReadInt();
            for (int i = 0; i < length; i++) {
                auto primitive = DecodeModelMeshOptionsPrimitiveObj(buffer);
                if (!primitive) {
                    MLOGE("model mesh options decode primitives fail");
                    return nullptr;
                }
                node->primitives.push_back(primitive);
            }
        }
            break;
                
        case 3:
            node->parent = new MInt(buffer->ReadInt());
            break;
                
        default:
            MLOGE("model mesh options invalid tag %u", key);
            return nullptr;
            break;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("modelMeshContentOptions", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

ContentDataBase* ModelContentDecoder::DecodeMeshContent(MBuffer* buffer) {
    PRINT_DATA_STRING("modelMeshContent", "begin--------------------------");
    ModelMeshContentData* node = new ModelMeshContentData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ModelMeshContentData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
            node->options = DecodeModelMeshOptions(buffer);
            if (!node->options) {
                MLOGE("model mesh decode options fail");
                return nullptr;
            }
            break;
                
        default:
            MLOGE("model mesh content invalid tag %u", key);
            return nullptr;
            break;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("modelMeshContent", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

static ModelAnimTrackOptionsData* DecodeModelAnimTrackOptions(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeModelAnimTrackOptions", "begin--------------------------");
    ModelAnimTrackOptionsData* node = new ModelAnimTrackOptionsData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ModelAnimTrackOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
            node->node = buffer->ReadInt();
            break;
            
        case 2:
        {
            node->input = DecodeBinaryData(buffer, node->input_length, nullptr);
            if (!node->input) {
                MLOGE("decode anim track input fail");
                return nullptr;
            }
        }
            break;
            
        case 3:
        {
            node->output = DecodeBinaryData(buffer, node->output_length, nullptr);
            if (!node->output) {
                MLOGE("decode anim track output fail");
                return nullptr;
            }
        }
            break;
            
        case 4:
            node->path = buffer->ReadString();
            break;
            
        case 5:
            node->interpolation = buffer->ReadString();
            break;
                
        default:
            MLOGE("model mesh content invalid tag %u", key);
            return nullptr;
            break;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("DecodeModelAnimTrackOptions", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

static ModelAnimationOptionsData* DecodeModelAnimationOptions(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeModelAnimationOptions", "begin--------------------------");
    ModelAnimationOptionsData* node = new ModelAnimationOptionsData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ModelAnimationOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
            node->name = buffer->ReadString();
            break;
                
        case 2:
        {
            int length = buffer->ReadInt();
            for (int i = 0; i < length; i++) {
                ModelAnimTrackOptionsData* track = DecodeModelAnimTrackOptions(buffer);
                if (!track) {
                    MLOGE("DecodeModelAnimationOptions decode track fail %d", i);
                    return nullptr;
                }
                node->tracks.push_back(track);
            }
        }
            break;
                
        default:
            MLOGE("DecodeTreeNodeOptions invalid tag %u", key);
            return nullptr;
            break;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("DecodeModelAnimationOptions", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

static TreeNodeOptionsData* DecodeTreeNodeOptions(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeTreeNodeOptions", "begin--------------------------");
    TreeNodeOptionsData* node = new TreeNodeOptionsData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        TreeNodeOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
            node->name = buffer->ReadString();
            break;
            
        case 2:
            node->transform = DecodeItemTransform(buffer);
            if (!node->transform) {
                return nullptr;
            }
            break;
            
        case 3:
            node->children_length = buffer->ReadInt();
            node->children = buffer->ReadIntArray(node->children_length);
            break;
            
        case 4:
            node->id = buffer->ReadString();
            break;
                
        default:
            MLOGE("DecodeTreeNodeOptions invalid tag %u", key);
            return nullptr;
            break;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("DecodeTreeNodeOptions", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

static ModelTreeOptionsData* DecodeModelTreeOptions(MBuffer* buffer) {
    PRINT_DATA_STRING("ModelTreeOptions", "begin--------------------------");
    ModelTreeOptionsData* node = new ModelTreeOptionsData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ModelTreeOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
            node->animation = buffer->ReadInt();
            break;
                
        case 2:
        {
            int length = buffer->ReadInt();
            for (int i = 0; i < length; i++) {
                ModelAnimationOptionsData* ani = DecodeModelAnimationOptions(buffer);
                if (!ani) {
                    MLOGE("model tree options decode ani fail %d", i);
                    return nullptr;
                }
                node->animations.push_back(ani);
            }
        }
            break;
                
        case 3:
        {
            int length = buffer->ReadInt();
            for (int i = 0; i < length; i++) {
                TreeNodeOptionsData* n = DecodeTreeNodeOptions(buffer);
                if (!n) {
                    MLOGE("model tree options decode node fail %d", i);
                    return nullptr;
                }
                node->nodes.push_back(n);
            }
        }
            break;
                
        case 4:
            node->children_length = buffer->ReadInt();
            node->children = buffer->ReadIntArray(node->children_length);
            break;
                
        default:
            MLOGE("model tree options invalid tag %u", key);
            return nullptr;
            break;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("ModelTreeOptions", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

static ModelTreeItemOptionsData* DecodeModelTreeItemOptions(MBuffer* buffer) {
    PRINT_DATA_STRING("ModelTreeItemOptions", "begin--------------------------");
    ModelTreeItemOptionsData* node = new ModelTreeItemOptionsData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ModelTreeItemOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
            node->tree = DecodeModelTreeOptions(buffer);
            if (!node->tree) {
                MLOGE("model tree content options tree fail");
                return nullptr;
            }
            break;
                
        default:
            MLOGE("model tree content options invalid tag %u", key);
            return nullptr;
            break;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("ModelTreeItemOptions", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

ContentDataBase* ModelContentDecoder::DecodeTreeContent(MBuffer* buffer) {
    PRINT_DATA_STRING("modelTreeContent", "begin--------------------------");
    ModelTreeContentData* node = new ModelTreeContentData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ModelTreeContentData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
            node->options = DecodeModelTreeItemOptions(buffer);
            if (!node->options) {
                MLOGE("model tree content options fail");
                return nullptr;
            }
            break;
                
        default:
            MLOGE("model tree content invalid tag %u", key);
            return nullptr;
            break;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("modelTreeContent", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

static SkyboxOptionsData* DecodeSkyboxContentOptions(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeSkyboxContentOptions", "begin--------------------------");
    SkyboxOptionsData* node = new SkyboxOptionsData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        SkyboxOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
            node->renderable = (buffer->ReadUint32() > 0);
            break;
            
        case 2:
            node->intensity = buffer->ReadFloat();
            break;
            
        case 3:
            node->reflections_intensity = buffer->ReadFloat();
            break;
            
        case 4:
        {
            int arr_length = buffer->ReadInt();
            for (int i = 0; i < arr_length; i++) {
                std::vector<float> arr;
                int length = buffer->ReadInt();
                for (int j = 0; j < length; j++) {
                    arr.push_back(buffer->ReadFloat());
                }
                node->irradianceCoeffs.push_back(arr);
            }
        }
            break;
            
        case 5:
            node->diffuse_image = DecodeGLTFTexture(buffer);
            if (!node->diffuse_image) {
                MLOGE("decode skybox options diffuseImage fail");
                return nullptr;
            }
            break;
            
        case 6:
            node->specular_image = DecodeGLTFTexture(buffer);
            if (!node->specular_image) {
                MLOGE("decode skybox options specularImage fail");
                return nullptr;
            }
            break;
            
        case 7:
            node->specular_image_size = buffer->ReadFloat();
            break;
            
        case 8:
            node->specular_mip_count = buffer->ReadInt();
            break;
                
        default:
            MLOGE("skybox options invalid tag %u", key);
            return nullptr;
            break;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("DecodeSkyboxContentOptions", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

static ModelLightOptionsData* DecodeLightContentOption(MBuffer* buffer) {
    PRINT_DATA_STRING("DecodeLightContentOption", "begin--------------------------");
    ModelLightOptionsData* node = new ModelLightOptionsData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ModelLightOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
            case 1:
                node->light_type = buffer->ReadString();
                break;
            case 2:
                node->parent = new MInt(buffer->ReadInt());
                break;
            case 3:
            {
                int arr_length = buffer->ReadInt();
                for(int i=0; i<arr_length; i++) {
                    float color = buffer->ReadFloat() / 255.0f;
                    node->color.push_back(color);
                }
            }
                break;
            case 4:
                node->intensity = buffer->ReadFloat();
                break;
            case 5:
                node->range = buffer->ReadFloat();
                break;
            case 6:
                node->innerConeAngle = buffer->ReadFloat();
                break;
            case 7:
                node->outerConeAngle = buffer->ReadFloat();
                break;
            default:
                MLOGE("light option invalid tag %u", key);
                break;
        }
        
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("DecodeLightContentOption", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

ContentDataBase* ModelContentDecoder::DecodeSkyboxContent(MBuffer* buffer) {
    PRINT_DATA_STRING("modelSkyboxContent", "begin--------------------------");
    SkyboxContentData* node = new SkyboxContentData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        SkyboxContentData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while (key != 0) {
        switch (key) {
        case 1:
            node->options = DecodeSkyboxContentOptions(buffer);
            if (!node->options) {
                MLOGE("skybox content options fail");
                return nullptr;
            }
            break;
                
        default:
            MLOGE("skybox content invalid tag %u", key);
            return nullptr;
            break;
        }
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("modelSkyboxContent", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

ContentDataBase* ModelContentDecoder::DecodeLightContent(MBuffer* buffer) {
    PRINT_DATA_STRING("modelLightContent", "begin--------------------------");
    ModelLightContentData* node = new ModelLightContentData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ModelLightContentData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;
    
    uint32_t key = buffer->ReadUint32();
    while(key != 0) {
        switch (key) {
            case 1:
                node->options = DecodeLightContentOption(buffer);
                break;
            default:
                MLOGE("light content invalid tag %u", key);
                return nullptr;
                break;
        }
        
        key = buffer->ReadUint32();
    }
    PRINT_DATA_STRING("modelLightContent", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

}
