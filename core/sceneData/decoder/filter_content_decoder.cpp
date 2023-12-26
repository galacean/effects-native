//
//  filter_content_decoder.cpp
//  MarsNative
//
//  Created by Zongming Liu on 2022/8/2.
//  Copyright © 2022 Alipay. All rights reserved.
//

#include "filter_content_decoder.hpp"
#include "sceneData/decoder/common_properties_decoder.hpp"

namespace mn {

ItemFilterData* FilterContentDecoder::GaussionFilterContentDecode(MBuffer* buffer) {
    GaussianFilterData* filter_data = new GaussianFilterData;

    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ItemFilterData* ptr_ = nullptr;
    };
    
    Holder holder;
    holder.ptr_ = filter_data;
    
    DataTag tag;
    tag.val = buffer->ReadInt();
    
    while (tag.val != 0) {
        switch (tag.i.idx) {
            case 2:
                if (data_type_is_int) {
                    filter_data->radius = new MInt(buffer->ReadInt());
                } else catch_data_exception(radius);
                break;
            default:
                break;
        }
        
        tag.val = buffer->ReadInt();
    }

    holder.ptr_ = nullptr;
    return filter_data;
}

ItemFilterData* FilterContentDecoder::DelayFilterContentDecode(MBuffer* buffer) {
    DelayFilterData* filter_data = new DelayFilterData;

    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ItemFilterData* ptr_ = nullptr;
    };
    
    Holder holder;
    holder.ptr_ = filter_data;
    
    DataTag tag;
    tag.val = buffer->ReadInt();
    
    while (tag.val != 0) {
        switch (tag.i.idx) {
            case 13:
                if (data_type_is_numbder_exp) {
                    filter_data->strength = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                } else catch_data_exception(strength);
                break;
            default:
                break;
        }
        
        tag.val = buffer->ReadInt();
    }
    
    holder.ptr_ = nullptr;
    return filter_data;
}

ItemFilterData* FilterContentDecoder::BloomFilterContentDecode(MBuffer* buffer) {
    BloomFilterData* filter_data = new BloomFilterData;

    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ItemFilterData* ptr_ = nullptr;
    };
    
    Holder holder;
    holder.ptr_ = filter_data;
    
    DataTag tag;
    tag.val = buffer->ReadInt();
    
    while (tag.val != 0) {
        switch (tag.i.idx) {
            case 8:
                if (data_type_is_int) {
                    filter_data->radius = new MInt(buffer->ReadInt());
                } else catch_data_exception(radius);
                break;
            case 9:
                if (data_type_is_floatarray) {
                    filter_data->color_weight = buffer->ReadFloatArray(3);
                } else catch_data_exception(color_weight);
                break;
            case 10:
                if (data_type_is_numbder_exp) {
                    filter_data->threshold = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                } else catch_data_exception(threshold);
                break;
            case 11:
                if (data_type_is_numbder_exp) {
                    filter_data->bloom_add_on = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                } else catch_data_exception(bloom_add_on);
                break;
            case 12:
                if (data_type_is_numbder_exp) {
                    filter_data->color_add_on = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                } else catch_data_exception(color_add_on);
                break;
            default:
                break;
        }
        
        tag.val = buffer->ReadInt();
    }
    
    holder.ptr_ = nullptr;
    return filter_data;
}

ItemFilterData* FilterContentDecoder::DistortionFilterContentDecode(MBuffer* buffer) {
    DistortionFilterData* filter_data = new DistortionFilterData;

    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        ItemFilterData* ptr_ = nullptr;
    };
    
    Holder holder;
    holder.ptr_ = filter_data;
    
    
    DataTag tag;
    tag.val = buffer->ReadInt();
    
    while (tag.val != 0) {
        switch (tag.i.idx) {
            case 3:
                if (data_type_is_floatarray) {
                    filter_data->center = buffer->ReadFloatArray(2);
                } else catch_data_exception(center);
                break;
            case 4:
                if (data_type_is_floatarray) {
                    filter_data->direction = buffer->ReadFloatArray(2);
                } else catch_data_exception(direction);
                break;
            case 5:
                if (data_type_is_numbder_exp) {
                    filter_data->strength = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                } else catch_data_exception(strength);
                break;
            case 6:
                if (data_type_is_numbder_exp) {
                    filter_data->period = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                } else catch_data_exception(period);
                break;
            case 7:
                if (data_type_is_numbder_exp) {
                    filter_data->wave_movement = CommonPropertiesDecoder::DecodeNumberExpression(buffer);
                } else catch_data_exception(wave_movement);
                break;
            default:
                break;
        }
        
        tag.val = buffer->ReadInt();
    }
    
    holder.ptr_ = nullptr;
    return filter_data;
}

}
