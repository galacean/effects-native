//
//  plugin_content_decoder.cpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/9/15.
//

#include "plugin_content_decoder.h"

namespace mn {

static PluginContentOptionsData* DecodePluginContentOptions(MBuffer* buffer) {
    PRINT_DATA_STRING("PluginContentOptions", "begin--------------------------");
    
    int type;
    DataTag tag;
    tag.val = buffer->ReadInt();
    if (tag.val == 0) {
        PRINT_DATA_STRING("PluginContentOptions", "no plugin type");
        return nullptr;
    }
    if (data_type_is_int) {
        type = buffer->ReadInt();
        PRINT_DATA_INT("item options type", options->type);
    } else catch_data_exception(type);

    PluginContentOptionsData* options = nullptr;
    switch (type) {
        case 1:
            options = new PluginARContentOptionsData;
            break;
            
        default:
            PRINT_DATA_STRING("PluginContentOptions", "invalid plugin type");
            return nullptr;
    }
    
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        PluginContentOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = options;

    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 2:
            if (data_type_is_string) {
                PluginARContentOptionsData* opt = ((PluginARContentOptionsData*) options);
                opt->track_type = buffer->ReadString();
                PRINT_DATA_STRING("item options type", opt->track_type.c_str());
                if (opt->track_type.compare("2d_face") == 0) {
                    opt->items_count = buffer->ReadInt();
                    if (opt->items_count) {
                        opt->items = new PluginARContentOptions2DItemData[opt->items_count];
                        for (int i = 0; i < opt->items_count; i++) {
                            opt->items[i].item_id = buffer->ReadString();
                            int* temp = buffer->ReadIntArray(3);
                            opt->items[i].track_index = temp[0];
                            opt->items[i].track_start = temp[1];
                            opt->items[i].track_end = temp[2];
                        }
                    }
                }
            } else catch_data_exception(type);
            break;

        default:
            MLOGE("plugin content options invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("PluginContentOptions", "end--------------------------");
    holder.ptr_ = nullptr;
    return options;
}

PluginContentData* PluginContentDecoder::Decode(MBuffer* buffer) {
    PRINT_DATA_STRING("PluginContent", "begin--------------------------");

    PluginContentData* node = new PluginContentData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        PluginContentData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_object) {
                node->options = DecodePluginContentOptions(buffer);
                if (!node->options) {
                    return nullptr;
                }
            } else catch_data_exception(options);
            break;
        
        default:
            MLOGE("plugin content invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("PluginContent", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

}
