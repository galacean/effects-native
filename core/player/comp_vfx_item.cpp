#include "comp_vfx_item.h"
#include <math.h>
#include "util/log_util.hpp"
#include "player/plugin_system.h"
#include "player/composition.h"

namespace mn {

static std::shared_ptr<VFXItem> CreateVFXItem(ItemData* options, Composition* composition) {
    // todo: add pluginName in loader
    int type_id = options->type;
    std::string type;
    switch (type_id) {
    case ITEM_CONTENT_TYPE_PARTICLE:
        type = "particle";
        break;
    case ITEM_CONTENT_TYPE_SPRITE :
        type = "sprite";
        break;
    case ITEM_CONTENT_TYPE_FILTER:
        type = "sprite";
        break;
    case ITEM_CONTENT_TYPE_CALCULATE:
        type = "cal";
        break;
    case ITEM_CONTENT_TYPE_MODEL_MESH:
        type = "model";
        break;
    case ITEM_CONTENT_TYPE_MODEL_TREE:
        type = "tree";
        break;
    case ITEM_CONTENT_TYPE_MODEL_SKYBOX:
        type = "model";
        break;
    case ITEM_CONTENT_TYPE_MODEL_LIGHT:
        type = "model";
        break;
    case ITEM_CONTENT_TYPE_INTERACT:
        type = "interact";
        break;
    case ITEM_CONTENT_TYPE_PLUGIN:
        type = "plugin";
        break;
    default:
        break;
    }
    if (type.empty()) {
        MLOGE("invalid vfx item type");
        assert(0);
    }
    return composition->plugin_system_->CreatePluginItem(type, options, composition);
}

CompVFXItem::CompVFXItem(ItemData* arg_t, Composition* comp) : VFXItem() {
    Create(arg_t, comp, true);
}

CompVFXItem::~CompVFXItem() {
    DEBUG_MLOGD("CompVFXItem::~CompVFXItem");
    MN_SAFE_DELETE(_items_);
}

void CompVFXItem::OnConstructed(ItemData* arg) {
    DEBUG_MLOGD("CompVFXItem::OnConstructed");
    CompositionData* options = (CompositionData*) arg;

    item_cache_map_.clear();
    items_ = &options->items;
    if (end_behavior_ == END_BEHAVIOR_RESTART || end_behavior_ == END_BEHAVIOR_PAUSE || end_behavior_ == END_BEHAVIOR_PAUSE_AND_DESTROY) {
        freeze_on_end_ = true;
    }
    start_time_in_ms_ = 0; // todo: round((this.startTime || 0) * 1000);
    items_to_remove_.clear();
}

float CompVFXItem::GetUpdateTime(float t) {
    float start_time = start_time_in_ms_;
    float now = time_in_ms_;
    if (t < 0 && (now + t) < start_time) {
        return start_time - now;
    }
    if (freeze_on_end_) {
        float remain = dur_in_ms_ - now;
        if (remain < t) {
            return remain;
        }
    }
    return roundf(t);
}

void CompVFXItem::OnUpdate(float dt, std::shared_ptr<VFXItem> thiz) {
    VFXItem::InnerOnUpdate(dt, thiz);
}

void CompVFXItem::OnItemUpdate(float dt, float lifetime) {
    const auto items = _items_;
    if (items) {
        composition_->UpdatePluginLoaders(dt);
        for (int i = 0; i < items->size(); i++) {
            const auto& item = items->at(i);
            if (item) {
                item->InnerOnUpdate(dt, item);
            }
            if (!item->composition_) {
                bool added = false;
                for (auto it : items_to_remove_) {
                    if (it == item) {
                        added = true;
                        break;
                    }
                }
                if (!added) {
                    items_to_remove_.push_back(item);
                }
            }
        }
        if (items_to_remove_.size()) {
            for (auto item : items_to_remove_) {
                for (auto iter = items->begin(); iter != items->end(); iter++) {
                    if (*iter == item) {
                        items->erase(iter);
                        break;
                    }
                }
            }
            items_to_remove_.clear();
        }
    }
}

void CompVFXItem::InnerStop() {
    if (_items_) {
        for (auto& item : *_items_) {
            item->Stop();
        }
    }
}

void CompVFXItem::OnItemRemoved(Composition* renderer, void* content) {
    if (_items_) {
        for (auto& item : *_items_) {
            item->Destroy(item);
        }
        MN_SAFE_DELETE(_items_);
    }
}

BaseContentItem* CompVFXItem::CreateContent() {
    if (!_items_) {
        _items_ = CreateItemsWidthOrder(composition_);
    }
    return nullptr;
}

void CompVFXItem::OnLifeTimeBegin(Composition* renderer, void* content) {
    for (auto& item : *_items_) {
        
        item->Start();
    }
}

std::vector<std::shared_ptr<VFXItem>>* CompVFXItem::CreateItemsWidthOrder(Composition* renderer) {
    std::vector<std::shared_ptr<VFXItem>>* ret = new std::vector<std::shared_ptr<VFXItem>>;
    for (int i = 0; i < items_->size(); i++) {
        std::shared_ptr<VFXItem> item = CreateVFXItem(items_->at(i), renderer);
        assert(item);
        ret->push_back(item);
        if (ret->size() > 1) {
            for (int j = ret->size() - 2; j >= 0; j--) {
                if (ret->at(j)->v_priority_ > item->v_priority_) {
                    ret->at(j + 1) = ret->at(j);
                    ret->at(j) = item;
                }
            }
        }
    }
    return ret;
}

std::shared_ptr<VFXItem> CompVFXItem::GetItemById(const std::string& id) {
    auto iter = item_cache_map_.find(id);
    std::shared_ptr<VFXItem> ret = nullptr;
    if (iter == item_cache_map_.end()) {
        std::shared_ptr<VFXItem> item;
        for (int i = 0; i < _items_->size(); i++) {
            if (_items_->at(i)->id_ == id) {
                item = _items_->at(i);
                break;
            }
        }
        if (item) {
            item_cache_map_[id] = item;
            ret = item;
        }
    } else {
        ret = iter->second;
    }
    return ret;
}

bool CompVFXItem::RemoveItem(std::shared_ptr<VFXItem> item) {
    if (!_items_) {
        return false;
    }
    for (auto& it : *_items_) {
        if (it == item) {
            item_cache_map_.erase(item->id_);
            items_to_remove_.push_back(item);
            return true;
        }
    }
    return false;
}

}
