#ifndef MN_SCENEDATA_DATA_ITEM_H_
#define MN_SCENEDATA_DATA_ITEM_H_

#include "contents.h"
#include "util/constant.hpp"

namespace mn {

struct ItemTransformData {
    float* position = nullptr;
    float* rotation = nullptr;
    float* scale = nullptr;
    float* quat = nullptr;
};

struct ItemData {
    std::string id;
    std::string name;
    float duration = 0;
    int type = 0;
    std::string parent_id;
    MBool* visible = nullptr;
    MInt* end_behavior = nullptr;
    float delay = 0;
    ContentDataBase* content = nullptr;
    RENDER_LEVEL render_level = RENDER_LEVEL_B_PLUS;
    MInt* pn = nullptr;
    std::string plugin_name;
    ItemTransformData* transform = nullptr;
    bool reusable = false;
    int list_index = 0;

    virtual ~ItemData() {
        MN_SAFE_DELETE(visible);
        MN_SAFE_DELETE(end_behavior);
        MN_SAFE_DELETE(content);
        MN_SAFE_DELETE(pn);
        MN_SAFE_DELETE(transform);
    }
};

}

#endif
