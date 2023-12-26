#include "sprite_group.h"
#include <math.h>
#include <algorithm>
#include "player/vfx_item.h"
#include "sprite_vfx_item.h"
#include "player/composition.h"

namespace mn {

static bool IsSprite(VFXItem* item) {
    auto type = item->GetType();
    return type.compare(VFX_ITEM_TYPE_SPRITE) == 0 || type.compare(VFX_ITEM_TYPE_FILTER_SPRITE) == 0;
}

static void ArrAddWithOrder(std::vector<std::shared_ptr<VFXItem>>& arr, std::shared_ptr<VFXItem> item) {
    arr.push_back(item);
    int idx = arr.size() - 1;
    while (idx > 0) {
        if (arr[idx - 1]->list_index_ > arr[idx]->list_index_) {
            auto temp = arr[idx - 1];
            arr[idx - 1] = arr[idx];
            arr[idx] = temp;
        } else {
            break;
        }
        idx--;
    }
}

static void ObjAssignMeshSplit(std::shared_ptr<MeshSplit> a, std::shared_ptr<MeshSplit> b) {
    a->index_start = b->index_start;
    a->render_info = b->render_info;
    a->cache_id = b->cache_id;
    a->items.clear();
    for (auto i : b->items) {
        a->items.push_back(i);
    }
    if (b->dirty != nullptr) {
        a->SetDirty(b->dirty->val);
    }
    if (b->index_end != nullptr) {
        a->SetIndexEnd(b->index_end->val);
    }
    if (b->sprite_mesh != nullptr) {
        a->sprite_mesh = b->sprite_mesh;
    }
}

SpriteGroup::SpriteGroup(CalculateGroup* calculate_group) : calculate_group_(calculate_group) {
}

SpriteGroup::~SpriteGroup() {
}

void SpriteGroup::ResetMeshSplits() {
    mesh_splits_.clear();
    meshes_.clear();
    items_.clear();
    items_to_add_.clear();
    items_to_remove_.clear();
}

void SpriteGroup::DiffMeshSplits(MeshSplitsDiff& diff) {
    auto& splits = mesh_splits_;
    std::vector<std::shared_ptr<MeshSplit>> splits_to_remove;
    std::vector<std::shared_ptr<MeshSplit>> combine;
    
    for (int i = 0; i < items_to_remove_.size(); i++) {
        auto& item = items_to_remove_[i];
        if (IsSprite(item.get())) {
            std::vector<std::shared_ptr<MeshSplit>> temp_rm;
            RemoveMeshSplitsItem(temp_rm, items_, std::static_pointer_cast<SpriteVFXItem>(item), splits, &items_to_remove_);
            for (auto s : temp_rm) {
                splits_to_remove.push_back(s);
            }
            // this._check
        } else {
            int item_index = Utils::GetVectorIndexOfItem(items_, item);
            if (item_index > -1) {
                Utils::VectorSplice((std::vector<std::shared_ptr<VFXItem>>*)nullptr, items_, item_index, 1);
                combine.clear();
                if (item_index > 0) {
                    CombineSplits(combine, items_, item_index - 1, splits);
                    for (auto s : combine) {
                        splits_to_remove.push_back(s);
                    }
                }
                if (!combine.size() && item_index < items_.size()) {
                    CombineSplits(combine, items_, item_index, splits);
                    for (auto s : combine) {
                        splits_to_remove.push_back(s);
                    }
                }
                // this._check
            }
        }
    }
    
    for (int i = 0; i < items_to_add_.size(); i++) {
        auto item = items_to_add_[i];
        std::vector<std::shared_ptr<MeshSplit>> neo_splits;
        AddMeshSplitsItem(neo_splits, items_, item, splits);
        for (int j = 0; j < neo_splits.size(); j++) {
            auto neo_split = neo_splits[j];
            if (neo_split->sprite_mesh) {
                MLOGE("no sprite mesh in neo split");
                assert(0);
                return;
            }
            neo_split->sprite_mesh = std::make_shared<SpriteMesh>(neo_split->render_info, composition_->renderer_, this->calculate_group_);
            auto sp = neo_split->sprite_mesh;
            diff.add.push_back(sp->mesh_);
            std::vector<SpriteItem*> temp;
            for (auto s : neo_split->items) {
                temp.push_back((SpriteItem*) s->content_);
            }
            sp->SetItems(temp);
            sp->ApplyChange();
            if (sp->split_layer_) {
                diff.layer.push_back(sp);
            }
            
            neo_split->SetDirty(false);
            // this._check();
        }
    }
    diff.remove.clear();
    for (int i = 0; i < splits.size(); i++) {
        auto split = splits[i];
        std::shared_ptr<SpriteMesh> sprite_mesh = split->sprite_mesh;
        if (split->items.size() == 0) {
            MLOGE("split not combined");
            assert(0);
            return;
        }
        if (split->dirty && split->dirty->val) {
            int priority = split->index_start;
            if (sprite_mesh->mesh_->priority_ != priority) {
                diff.modify.push_back(sprite_mesh->mesh_);
            }
            std::vector<SpriteItem*> temp;
            for (auto s : split->items) {
                temp.push_back((SpriteItem*) s->content_);
            }
            sprite_mesh->SetItems(temp);
        }
        sprite_mesh->ApplyChange();
        split->SetDirty(false);
    }
    if (splits_to_remove.size()) {
        for (int i = 0; i < splits_to_remove.size(); i++) {
            auto split = splits_to_remove[i];
            auto mesh = split->sprite_mesh->mesh_;
            mesh->Destroy();
            diff.remove.push_back(mesh);
        }
    }
    items_to_remove_.clear();
    items_to_add_.clear();
    if (diff.add.size() + diff.remove.size() + diff.modify.size()) {
        auto& ms = meshes_;
        ms.clear();
        int i = 0;
        for (auto split : mesh_splits_) {
            ms.push_back(split->sprite_mesh->mesh_);
            i++;
        }
    }
}

void SpriteGroup::AddItem(std::shared_ptr<VFXItem> vfx_item) {
    if (Utils::GetVectorIndexOfItem(items_, vfx_item) == -1) {
        Utils::VectorAdd(items_to_add_, vfx_item);
    } else {
        Utils::VectorRemove(items_to_remove_, vfx_item);
    }
}

void SpriteGroup::RemoveItem(std::shared_ptr<VFXItem> vfx_item) {
    if (Utils::GetVectorIndexOfItem(items_, vfx_item) > -1) {
        Utils::VectorAdd(items_to_remove_, vfx_item);
    } else {
        Utils::VectorRemove(items_to_add_, vfx_item);
    }
}

std::shared_ptr<SpriteMesh> SpriteGroup::GetSpriteMesh(SpriteItem* item) {
    auto& meshes = mesh_splits_;
    for (int i = 0; i < meshes.size(); i++) {
        auto mesh = meshes[i]->sprite_mesh;
        int item_index = Utils::GetVectorIndexOfItem(mesh->items_, item);
        if (item_index > -1) {
            return mesh;
        }
    }
    return nullptr;
}

void SpriteGroup::OnUpdate(float dt) {
    time_ += dt / 1000.0f;
    auto& splits = mesh_splits_;
    for (int i = 0; i < splits.size(); i++) {
        auto mesh = splits[i]->sprite_mesh;
        auto& items = mesh->items_;

        for (int j = 0; j < items.size(); j++) {
            SpriteItem* item = items[j];
            item->UpdateTime(time_);
            if (!item->GetEnded()) {
                mesh->UpdateItem(item);
            }
        }
        mesh->ApplyChange();
    }
}

void SpriteGroup::Destroy() {
    for (auto mesh : mesh_splits_) {
        mesh->sprite_mesh->mesh_->Destroy();
    }
}

void SpriteGroup::RemoveMeshSplitsItem(std::vector<std::shared_ptr<MeshSplit>>& out,
        std::vector<std::shared_ptr<VFXItem>>& items, std::shared_ptr<SpriteVFXItem> item, std::vector<std::shared_ptr<MeshSplit>>& splits,
        std::vector<std::shared_ptr<VFXItem>>* items_to_remove) {
    std::shared_ptr<MeshSplit> target_split;
    int target_split_index = 0;
    for (int i = 0; i < splits.size(); i++) {
        auto split = splits[i];
        if (split->index_start <= item->list_index_ && split->index_end->val >= item->list_index_) {
            target_split = split;
            target_split_index = i;
        }
    }
    if (target_split) {
        int index = -1;
        for (int i = 0; i < target_split->items.size(); i++) {
            if (target_split->items[i] == item) {
                index = i;
                break;;
            }
        }
        if (index < 0) {
            if (items_to_remove) {
                for (int i = 0; i < items_to_remove->size(); i++) {
                    if (items_to_remove->at(i) == item) {
                        return;
                    }
                }
            }
            MLOGE("item not found");
            assert(0);
            return;
        }
        for (auto iter = target_split->items.begin(); iter != target_split->items.end(); iter++) {
            if (*iter == item) {
                target_split->items.erase(iter);
                break;
            }
        }
        target_split->SetDirty(true);
        for (auto iter = items.begin(); iter != items.end(); iter++) {
            if (*iter == item) {
                items.erase(iter);
                break;
            }
        }
        if (target_split->items.size() == 0) {
            for (auto iter = splits.begin(); iter != splits.end(); iter++) {
                if (*iter == target_split) {
                    splits.erase(iter);
                    break;
                }
            }
            out.push_back(target_split);
            target_split_index = target_split_index - 1;
            if (!splits.size() || target_split_index < 0) {
                return;
            }
            target_split = splits[target_split_index];
        } else {
            target_split->SetIndexEnd(target_split->items[target_split->items.size() - 1]->list_index_);
            target_split->index_start = target_split->items[0]->list_index_;
        }
        if (target_split_index == 0 || target_split_index == splits.size() - 2) {
            //combine next
            std::shared_ptr<MeshSplit> p0 = splits[target_split_index];
            std::shared_ptr<MeshSplit> p1;
            if (target_split_index + 1 < splits.size()) {
                p1 = splits[target_split_index + 1];
            }
            if (p0 && p1) {
                auto i0 = p0->items[0];
                auto i1 = p1->items[p1->items.size() - 1];
                std::vector<std::shared_ptr<MeshSplit>> meshes;
                int i0_idx = 0;
                int i1_idx = 0;
                for (int i = 0; i < items.size(); i++) {
                    if (items[i] == i0) {
                        i0_idx = i;
                    } else if (items[i] == i1) {
                        i1_idx = i;
                    }
                }
                GetMeshSplits(meshes, items, i0_idx, i1_idx);
                if (meshes.size() == 1) {
                    meshes[0]->sprite_mesh = splits[target_split_index]->sprite_mesh;
                    splits[target_split_index] = meshes[0];
                    int temp_idx = 0;
                    for (auto iter = splits.begin(); iter != splits.end(); iter++) {
                        if (temp_idx == target_split_index + 1) {
                            out.push_back(*iter);
                            splits.erase(iter);
                            break;
                        }
                        ++temp_idx;
                    }
                }
            }
        } else if (target_split_index == splits.size() - 1) {
            //combine preview
            if (target_split->items.size() == 0) {
                int temp_idx = 0;
                for (auto iter = splits.begin(); iter != splits.end(); iter++) {
                    if (temp_idx == target_split_index) {
                        out.push_back(*iter);
                        splits.erase(iter);
                        break;
                    }
                    ++temp_idx;
                }
            } else {
                auto p0 = splits[target_split_index - 1];
                auto p1 = splits[target_split_index];
                auto i0 = p0->items[0];
                auto i1 = p1->items[p1->items.size() - 1];
                std::vector<std::shared_ptr<MeshSplit>> meshes;
                int i0_idx = 0;
                int i1_idx = 0;
                for (int i = 0; i < items.size(); i++) {
                    if (items[i] == i0) {
                        i0_idx = i;
                    } else if (items[i] == i1) {
                        i1_idx = i;
                    }
                }
                GetMeshSplits(meshes, items, i0_idx, i1_idx);
                if (meshes.size() == 1) {
                    meshes[0]->sprite_mesh = splits[target_split_index - 1]->sprite_mesh;
                    splits[target_split_index - 1] = meshes[0];
                    int temp_idx = 0;
                    for (auto iter = splits.begin(); iter != splits.end(); iter++) {
                        if (temp_idx == target_split_index) {
                            out.push_back(*iter);
                            splits.erase(iter);
                            break;
                        }
                        ++temp_idx;
                    }
                }
            }
        } else {
            //combine preview and next
            auto p0 = splits[target_split_index - 1];
            auto p1 = splits[target_split_index + 1];
            auto i0 = p0->items[0];
            auto i1 = p1->items[p1->items.size() - 1];
            std::vector<std::shared_ptr<MeshSplit>> meshes;
            int i0_idx = 0;
            int i1_idx = 0;
            for (int i = 0; i < items.size(); i++) {
                if (items[i] == i0) {
                    i0_idx = i;
                } else if (items[i] == i1) {
                    i1_idx = i;
                }
            }
            GetMeshSplits(meshes, items, i0_idx, i1_idx);
            if (meshes.size() == 2) {
                meshes[0]->sprite_mesh = splits[target_split_index]->sprite_mesh;
                meshes[1]->sprite_mesh = splits[target_split_index + 1]->sprite_mesh;
                splits[target_split_index] = meshes[0];
                splits[target_split_index + 1] = meshes[1];
                int temp_idx = 0;
                for (auto iter = splits.begin(); iter != splits.end(); iter++) {
                    if (temp_idx == target_split_index - 1) {
                        out.push_back(*iter);
                        splits.erase(iter);
                        break;
                    }
                    ++temp_idx;
                }
            }
        }
    }
}

void SpriteGroup::GetMeshSplits(std::vector<std::shared_ptr<MeshSplit>>& ret,
        std::vector<std::shared_ptr<VFXItem>>& items, int start_index, int end_index, bool init) {
    std::shared_ptr<MeshSplit> current;
    
    for (int i = start_index; i <= end_index; i++) {
        auto item = items[i];
        if (!init && (!item->GetContentVisible() || item->lifetime_ < 0)) {
            continue;
        }
        if (!IsSprite(item.get())) {
            if (init && (!item->GetContentVisible())) {
                continue;
            }
            if (current) {
                ret.push_back(current);
                current.reset();
            }
        } else {
            std::string cache_id = ((SpriteItem*) item->CreateContent())->render_info_->cache_id;
            bool replace_current = true;
            if (current) {
                if (current->cache_id.compare(cache_id) == 0 && current->items.size() < SpriteMesh::GetMaxSpriteMeshItemCount()) {
                    current->items.push_back(std::static_pointer_cast<SpriteVFXItem>(item));
                    int idx = current->items.size() - 1;
                    while (idx > 0) {
                        if (current->items[idx - 1]->list_index_ > current->items[idx]->list_index_) {
                            auto temp = current->items[idx - 1];
                            current->items[idx - 1] = current->items[idx];
                            current->items[idx] = temp;
                        } else {
                            break;
                        }
                    }
                    replace_current = false;
                } else {
                    ret.push_back(current);
                }
            }
            if (replace_current) {
                current = std::make_shared<MeshSplit>();
                current->index_start = item->list_index_;
                current->cache_id = cache_id;
                current->render_info = ((SpriteItem*) item->content_)->render_info_;
                current->items.push_back(std::static_pointer_cast<SpriteVFXItem>(item));
            }
        }
    }
    if (current) {
        ret.push_back(current);
    }
    for (int i = 0; i < ret.size(); i++) {
        auto split = ret[i];
        split->SetIndexEnd(split->items[split->items.size() - 1]->list_index_);
        split->SetDirty(true);
    }
}

void SpriteGroup::CombineSplits(std::vector<std::shared_ptr<MeshSplit>>& ret, std::vector<std::shared_ptr<VFXItem>>& items, int item_index,
        std::vector<std::shared_ptr<MeshSplit>>& splits) {
    auto item = items[item_index];
    if (IsSprite(item.get())) {
        size_t target_split_index = 0;
        for (size_t i = 0; i < splits.size(); i++) {
            auto split = splits[i];
            auto temp_item = std::static_pointer_cast<SpriteVFXItem>(item);
            if (Utils::GetVectorIndexOfItem(split->items, temp_item) > -1) {
                target_split_index = i;
                break;
            }
        }
        std::shared_ptr<MeshSplit> p0;
        std::shared_ptr<MeshSplit> p1;
        if (splits.size() > 0) {
            if (target_split_index == 0) {
                p0 = splits[target_split_index];
                if (target_split_index + 1 < splits.size()) {
                    p1 = splits[target_split_index + 1];
                }
            } else {
                p0 = splits[target_split_index - 1];
                p1 = splits[target_split_index];
            }
        }
        if (p0 && p1) {
            std::shared_ptr<VFXItem> temp_ptr = p0->items[0];
            int start_index = Utils::GetVectorIndexOfItem(items, temp_ptr);
            temp_ptr = p1->items[p1->items.size() - 1];
            int end_index = Utils::GetVectorIndexOfItem(items, temp_ptr);
            
            std::vector<std::shared_ptr<MeshSplit>> meshes;
            GetMeshSplits(meshes, items, start_index, end_index);
            if (meshes.size() == 1) {
                meshes[0]->sprite_mesh = splits[target_split_index]->sprite_mesh;
                if (target_split_index == 0) {
                    splits[0] = meshes[0];
                    std::vector<std::shared_ptr<MeshSplit>> temp;
                    Utils::VectorSplice(&temp, splits, 1, 1);
                    ret.push_back(temp[0]);
                } else {
                    std::vector<std::shared_ptr<MeshSplit>> temp;
                    Utils::VectorSplice(&temp, splits, target_split_index - 1, 1);
                    ret.push_back(temp[0]);
                    splits[target_split_index - 1] = meshes[0];
                }
            }
        }
    }
}

void SpriteGroup::AddMeshSplitsItem(std::vector<std::shared_ptr<MeshSplit>>& ret,
        std::vector<std::shared_ptr<VFXItem>>& items, std::shared_ptr<VFXItem> item, std::vector<std::shared_ptr<MeshSplit>>& splits) {
    auto item_index = Utils::GetVectorIndexOfItem(items, item);
    if (item_index != -1) {
        MLOGE("item has been added");
        assert(0);
        return;
    }
    auto first_split = splits.size() > 0 ? splits[0] : nullptr;
    if (!first_split) {
        ArrAddWithOrder(items, item);
        if (IsSprite(item.get())) {
            SpriteItem* content = (SpriteItem*) item->CreateContent();
            std::shared_ptr<MeshSplit> split = std::make_shared<MeshSplit>();
            split->index_start = item->list_index_;
            split->SetIndexEnd(item->list_index_);
            split->items.push_back(std::static_pointer_cast<SpriteVFXItem>(item));
            split->render_info = content->render_info_;
            split->cache_id = content->render_info_->cache_id;
            splits.insert(splits.begin(), split);
            ret.push_back(split);
            return;
        }
        return;
    }
    for (int i = 0; i < splits.size(); i++) {
        auto split = splits[i];
        auto list_index = item->list_index_;
        if (IsSprite(item.get())) {
            if (list_index <= split->index_end->val) {
                ArrAddWithOrder(items, item);
                auto item_index = Utils::GetVectorIndexOfItem(items, item);
                std::shared_ptr<VFXItem> temp_ptr = split->items[0];
                int index_start = std::min(item_index, Utils::GetVectorIndexOfItem(items, temp_ptr));
                temp_ptr = split->items[split->items.size() - 1];
                int index_end = std::max(item_index, Utils::GetVectorIndexOfItem(items, temp_ptr));
                std::vector<std::shared_ptr<MeshSplit>> neo_splits;
                GetMeshSplits(neo_splits, items, index_start, index_end);
                int neo_split_index = -1;
                for (auto split : neo_splits) {
                    ++neo_split_index;
                    std::shared_ptr<SpriteVFXItem> temp_ptr = std::static_pointer_cast<SpriteVFXItem>(item);
                    if (Utils::GetVectorIndexOfItem(split->items, temp_ptr) > -1) {
                        break;
                    }
                }
                if (neo_splits.size() == 2) {
                    if (neo_split_index != 0) {
                        MLOGE("neo split not first");
                        assert(0);
                        return;
                    }
                    splits.insert(splits.begin() + i, neo_splits[0]);
                    ObjAssignMeshSplit(split, neo_splits[1]);
                    ret.push_back(neo_splits[neo_split_index]);
                    return;
                } else if (neo_splits.size() == 3) {
                    ObjAssignMeshSplit(split, neo_splits[0]);
                    splits.insert(splits.begin() + i + 1, neo_splits[2]);
                    splits.insert(splits.begin() + i + 1, neo_splits[1]);
                    if (neo_split_index != 1) {
                        MLOGE("neo split not in middle");
                        assert(0);
                        return;;
                    }
                    ret.push_back(neo_splits[1]);
                    ret.push_back(neo_splits[2]);
                    return;
                } else if (neo_splits.size() != 1) {
                    MLOGE("invalid splits 1");
                    assert(0);
                    return;;
                }
                ObjAssignMeshSplit(split, neo_splits[0]);
                return;
            }
        } else {
            if (list_index < split->index_start || list_index == split->index_end->val) {
                ArrAddWithOrder(items, item);
                return;
            } else if (list_index < split->index_end->val) {
                ArrAddWithOrder(items, item);
                std::shared_ptr<VFXItem> last_item = split->items[split->items.size() - 1];
                int end_index = Utils::GetVectorIndexOfItem(items, last_item);
                std::vector<std::shared_ptr<MeshSplit>> neo_splits;
                std::shared_ptr<VFXItem> temp_ptr = split->items[0];
                GetMeshSplits(neo_splits, items, Utils::GetVectorIndexOfItem(items, temp_ptr), end_index);
                ObjAssignMeshSplit(split, neo_splits[0]);
                if (neo_splits.size() == 2) {
                    splits.insert(splits.begin() + i + 1, neo_splits[1]);
                    ret.push_back(neo_splits[1]);
                    return;
                } else if (neo_splits.size() != 1) {
                    MLOGE("invalid splits 2");
                    assert(0);
                    return;
                }
            }
        }
    }
    ArrAddWithOrder(items, item);
    if (IsSprite(item.get())) {
        auto last = splits[splits.size() - 1];
        std::vector<std::shared_ptr<MeshSplit>> neo_splits;
        std::shared_ptr<VFXItem> temp_ptr = last->items[0];
        GetMeshSplits(neo_splits, items, Utils::GetVectorIndexOfItem(items, temp_ptr), Utils::GetVectorIndexOfItem(items, item));
        ObjAssignMeshSplit(last, neo_splits[0]);
        if (neo_splits.size() == 2) {
            splits.push_back(neo_splits[1]);
            ret.push_back(neo_splits[1]);
            return;
        } else if (neo_splits.size() != 1) {
            MLOGE("invalid splits 3");
            assert(0);
            return;
        }
    }
}

}
