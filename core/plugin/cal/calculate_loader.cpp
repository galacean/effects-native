#include "calculate_loader.h"
#include "calculate_vfx_item.h"
#include "player/comp_vfx_item.h"
#include "player/composition.h"
#include "plugin/tree/tree_item.hpp"

namespace mn {

static void SetItemParent(std::shared_ptr<VFXItem> item, std::shared_ptr<VFXItem> parent, std::shared_ptr<Transform> parent_transform = nullptr) {
    item->parent = parent;
    if (parent_transform) {
        item->transform_->SetParentTransform(parent_transform);
    } else if (parent) {
        item->transform_->SetParentTransform(parent->transform_);
    } else {
        item->transform_->SetParentTransform(nullptr);
    }
}

static std::string GetParentIdWithoutSubfix(const std::string& id) {
    auto idx = id.find_last_of("^");
    if (idx != std::string::npos) {
        return id.substr(0, idx);
    } else {
        return id;
    }
}

CalculateLoader::CalculateLoader() {
    order_ = 0;
    name_ = "CalculateLoader";
}

CalculateLoader::~CalculateLoader() {
}

void CalculateLoader::OnCompositionReset(Composition* comp, MarsPlayerRenderFrame* pipeline) {
    const auto& group = comp->calculate_group_;
    std::map<std::string, int> ref_count_map;
    auto& item_map = item_map_;
    for (int i = 0; i < comp->GetItems()->size(); i++) {
        std::shared_ptr<VFXItem> item = comp->GetItems()->at(i);
        if (item->GetType().compare(VFX_ITEM_TYPE_NULL) == 0) {
            group->AddItem((CalculateItem*) item->CreateContent());
            item_map[item->id_] = std::static_pointer_cast<CalculateVFXItem>(item);
        } else if (item->parent_id_.size()) {
            auto iter = ref_count_map.find(item->parent_id_);
            if (iter == ref_count_map.end()) {
                ref_count_map[item->parent_id_] = 1;
            } else {
                ref_count_map[item->parent_id_] = iter->second + 1;
            }
        }
        if (item->GetType().compare(VFX_ITEM_TYPE_TREE) == 0) {
            item_map[item->id_] = item;
        }
    }

    group->BuildTree(ref_count_map);
}

void CalculateLoader::SetItemsParentTransform(CalculateGroup* calculate_group, std::vector<std::shared_ptr<VFXItem>>* items, std::shared_ptr<VFXItem> item) {
    const std::string& id = item->id_;
    std::shared_ptr<Transform> pt = item->composition_->root_transform_;
    item->transform_->SetParentTransform(pt);
    if (item->GetType().compare(VFX_ITEM_TYPE_NULL) == 0) {
        for (int i = 0; i < items->size(); i++) {
            std::shared_ptr<VFXItem> child = items->at(i);
            if (id.compare(child->parent_id_) == 0 && child != item) {
                child->transform_->SetParentTransform(item->transform_);
                SetItemParent(child, item);
            }
        }
    } else if (item->GetType().compare(VFX_ITEM_TYPE_TREE) == 0) {
        const std::string& id = item->id_;
        const std::string id_width_subfix = id + "^";
        for (int i = 0; i < items->size(); i++) {
            std::shared_ptr<VFXItem> child = items->at(i);
            const std::string& parent_id = child->parent_id_;
            if (!parent_id.empty() && item != child) {
                if (id.compare(parent_id) == 0) {
                    SetItemParent(child, item);
                } else if (parent_id.find(id_width_subfix) == 0) {
                    auto t = ((Tree*) item->content_)->GetNodeTransform(parent_id.substr(id_width_subfix.size()));
                    SetItemParent(child, item, t);
                }
            }
        }
    }
    const std::string& parent_id = item->parent_id_;
    std::shared_ptr<VFXItem> parent_item;
    if (!parent_id.empty()) {
        auto iter = item_map_.find(GetParentIdWithoutSubfix(parent_id));
        if (iter != item_map_.end()) {
            parent_item = iter->second;
        }
    }
    if (parent_item && parent_item->GetLifetimeStarted()) {
        if (parent_item->GetType().compare(VFX_ITEM_TYPE_NULL) == 0 && ((CalculateItem*) parent_item->content_)->active_) {
            pt = parent_item->transform_;
        } else if (parent_item->GetType().compare(VFX_ITEM_TYPE_TREE) == 0) {
            pt = ((Tree*) parent_item->content_)->GetNodeTransform(parent_id.substr(parent_id.find("^") + 1));
        }
        SetItemParent(item, parent_item, pt);
    }
}

void CalculateLoader::OnCompositionUpdate(Composition* comp, float dt) {
    // todo: OnUpdateByLoader存在对comp->calculate_group_->item_map_的删除操作
    std::vector<std::string> keys;
    for (auto iter : item_map_) {
        keys.push_back(iter.first);
    }
    for (auto key : keys) {
        auto iter = item_map_.find(key);
        if (iter != item_map_.end()) {
            auto item = iter->second;
            if (item) {
                item->OnUpdateByLoader(dt, item);
            }
        }
    }
}

void CalculateLoader::OnCompositionItemLifeBegin(Composition* comp, std::shared_ptr<VFXItem> item) {
    if (item->GetType().compare(VFX_ITEM_TYPE_COMPOSITION) != 0) {
        SetItemsParentTransform(comp->calculate_group_, comp->GetItems(), item);
    }
}

void CalculateLoader::OnCompositionItemRemoved(Composition* comp, std::shared_ptr<VFXItem> item) {
    const auto& group = comp->calculate_group_;
    item_map_.erase(item->id_);
    if (item->GetType().compare(VFX_ITEM_TYPE_NULL) == 0) {
        group->RemoveItemRef(item->id_);
    } else if (item->parent_id_.size()) {
        group->RemoveItemRef(item->parent_id_);
    }
    std::vector<std::shared_ptr<VFXItem>> items_to_remove;
    if (item->GetType().compare(VFX_ITEM_TYPE_TREE) == 0 || item->GetType().compare(VFX_ITEM_TYPE_NULL) == 0) {
        const bool will_remove = (item->end_behavior_ == END_BEHAVIOR_TYPE::END_BEHAVIOR_DESTROY_CHILDREN);
        if (will_remove) {
            items_to_remove.clear();
        }
        const bool keep_parent = ((item->GetType().compare(VFX_ITEM_TYPE_NULL) == 0) && (group->item_map_.find(item->id_) != group->item_map_.end()));
        for (int i = 0; i < comp->GetItems()->size(); i++) {
            auto cit = comp->GetItems()->at(i);
            if (cit->parent == item) {
                if (!keep_parent) {
                    SetItemParent(cit, nullptr);
                    cit->transform_->SetParentTransform(comp->root_transform_);
                }
                if (will_remove) {
                    Utils::VectorAdd(items_to_remove, cit);
                }
            }
        }
    }
    if (group->item_map_.find(item->parent_id_) == group->item_map_.end()) {
        SetItemParent(item, nullptr);
    }
    if (items_to_remove.size()) {
        for (int i = 0; i < items_to_remove.size(); i++) {
            items_to_remove[i]->Destroy(items_to_remove[i]);
        }
    }
}

}
