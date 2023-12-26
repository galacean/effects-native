#include "interact_content_decoder.h"

namespace mn {

static InteractContentOptionsData* DecodeInteractContentOptions(MBuffer* buffer) {
    PRINT_DATA_STRING("InteractContentOptions", "begin--------------------------");

    InteractContentOptionsData* options = new InteractContentOptionsData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        InteractContentOptionsData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = options;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_int) {
                options->type = buffer->ReadInt();
                PRINT_DATA_INT("item options type", options->type);
            } else catch_data_exception(type);
            break;

        default:
            MLOGE("interace content options invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("InteractContentOptions", "end--------------------------");
    holder.ptr_ = nullptr;
    return options;
}

InteractContentData* InteractContentDecoder::Decode(MBuffer* buffer) {
    PRINT_DATA_STRING("InteractContent", "begin--------------------------");

    InteractContentData* node = new InteractContentData;
    class Holder {
    public:
        ~Holder() {
            MN_SAFE_DELETE(ptr_);
        }
        
        InteractContentData* ptr_ = nullptr;
    };
    Holder holder;
    holder.ptr_ = node;

    DataTag tag;
    tag.val = buffer->ReadInt();
    while (tag.val != 0) {
        switch (tag.i.idx) {
        case 1:
            if (data_type_is_object) {
                node->options = DecodeInteractContentOptions(buffer);
                if (!node->options) {
                    return nullptr;
                }
            } else catch_data_exception(options);
            break;
        
        default:
            MLOGE("interact content invalid tag %d", tag.val);
            return nullptr;
            break;
        }

        tag.val = buffer->ReadInt();
    }

    PRINT_DATA_STRING("InteractContent", "end--------------------------");
    holder.ptr_ = nullptr;
    return node;
}

}
