#include "loader.h"
#include "util/log_util.hpp"

namespace mn {

static bool PassRenderLevel(int l, int device_level) {
    if (!l || !device_level) {
        return true;
    }
    return (l & device_level) != 0;
}

VFXCompositionItem* Loader::GetVFXItem(SceneDataDecoder* scene, const GetVFXItemOptions& options) {
    int list_order = 0;
    auto composition = scene->GetComposition();

    END_BEHAVIOR_TYPE end_behavior = END_BEHAVIOR_TYPE::END_BEHAVIOR_DESTROY;
    if (composition->end_behavior) {
        end_behavior = CreateEndBehaviorType(composition->end_behavior->val);
    }
    bool looping = false;
    bool reusable = false;
    int config_mask = 0;

    auto& items = composition->items;
    std::vector<ItemData*> origin_items;
    origin_items.swap(items); // 移除renderLevel和deviceLevel不匹配的item
    for (int i = 0; i < origin_items.size(); i++) {
        const auto& item = origin_items[i];
        
        if (item->visible && item->visible->val == false) {
            delete item;
            continue;
        }

        int item_type = item->type;
        auto content = item->content;
        
        // 移除不认识的type
        if (item_type != ITEM_CONTENT_TYPE_PARTICLE && item_type != ITEM_CONTENT_TYPE_SPRITE
            && item_type != ITEM_CONTENT_TYPE_CALCULATE && item_type != ITEM_CONTENT_TYPE_FILTER
            && item_type != ITEM_CONTENT_TYPE_INTERACT && item_type != ITEM_CONTENT_TYPE_PLUGIN
            && item_type != ITEM_CONTENT_TYPE_MODEL_MESH && item_type != ITEM_CONTENT_TYPE_MODEL_TREE
            && item_type != ITEM_CONTENT_TYPE_MODEL_SKYBOX && item_type != ITEM_CONTENT_TYPE_MODEL_LIGHT) {
            MLOGE("invalid content type %d", item_type);
            delete item;
            assert(0);
            continue;
        }

        if (content) {
            if (PassRenderLevel(item->render_level, options.render_level)) {
                if (content->renderer) {
                    if (!content->renderer->mask || content->renderer->mask == 0) {
                        auto mask_mode = content->renderer->mask_mode;
                        if (mask_mode) {
                            if (mask_mode->val == MASK_MODE_WRITE_MASK) {
                                content->renderer->mask = new MInt(++config_mask);
                            } else if (mask_mode->val == MASK_MODE_READ_MASK || mask_mode->val == MASK_MODE_READ_INVERT_MASK) {
                                content->renderer->mask = new MInt(config_mask);
                            }
                        }
                    }
                }

                item->list_index = (++list_order);
                items.push_back(item);
                continue;
            }
        }
        delete item;
    }

    VFXCompositionItem* item = new VFXCompositionItem;
    item->end_behavior = end_behavior;
    item->looping = looping;
    item->camera = scene->GetComposition()->camera;

    return item;
}

}
