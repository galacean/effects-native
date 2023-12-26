#ifndef MN_PLUGIN_SPRITE_SPRITE_GROUP_H_
#define MN_PLUGIN_SPRITE_SPRITE_GROUP_H_

#include <string>
#include <vector>
#include "sceneData/scene_data_decoder.h"
#include "util/util.hpp"

namespace mn {

class CalculateGroup;
class SpriteVFXItem;
class SpriteItem;
class SpriteMesh;
class Composition;
class VFXItem;
class MarsMesh;
class SpriteItemRenderInfo;

struct MeshSplit {
    int index_start = 0;
    // todo: check leak;
    SpriteItemRenderInfo* render_info = nullptr;
    std::string cache_id;
    std::vector<std::shared_ptr<SpriteVFXItem>> items;
    
    // todo: refactor
    mn::MBool* dirty = nullptr;
    mn::MInt* index_end = nullptr;
    std::shared_ptr<SpriteMesh> sprite_mesh;
    
    ~MeshSplit() {
        MN_SAFE_DELETE(dirty);
        MN_SAFE_DELETE(index_end);
    }
    
    void SetDirty(bool val) {
        if (!dirty) {
            dirty = new mn::MBool(val);
        } else {
            dirty->val = val;
        }
    }
    
    void SetIndexEnd(int val) {
        if (!index_end) {
            index_end = new mn::MInt(val);
        } else {
            index_end->val = val;
        }
    }
};

struct MeshSplitsDiff {
    std::vector<std::shared_ptr<MarsMesh>> remove;
    std::vector<std::shared_ptr<MarsMesh>> add;
    std::vector<std::shared_ptr<MarsMesh>> modify;
    
    // todo: 前端数据结构奇怪
    std::vector<std::shared_ptr<SpriteMesh>> layer;
};

class SpriteGroup {
public:
    SpriteGroup(CalculateGroup* calculate_group);

    ~SpriteGroup();

    void ResetMeshSplits();

    void DiffMeshSplits(MeshSplitsDiff& diff);
    
    void AddItem(std::shared_ptr<VFXItem> vfx_item);
    
    void RemoveItem(std::shared_ptr<VFXItem> vfx_item);
    
    std::shared_ptr<SpriteMesh> GetSpriteMesh(SpriteItem* item);
    
    void OnUpdate(float dt);
    
    void Destroy();

private:
#ifdef UNITTEST
public:
#endif
    // todo: static?
    void RemoveMeshSplitsItem(std::vector<std::shared_ptr<MeshSplit>>& out,
        std::vector<std::shared_ptr<VFXItem>>& items, std::shared_ptr<SpriteVFXItem> item, std::vector<std::shared_ptr<MeshSplit>>& splits,
        std::vector<std::shared_ptr<VFXItem>>* items_to_remove = nullptr);

    void GetMeshSplits(std::vector<std::shared_ptr<MeshSplit>>& ret,
        std::vector<std::shared_ptr<VFXItem>>& items, int start_index, int end_index, bool init = false);

    void CombineSplits(std::vector<std::shared_ptr<MeshSplit>>& ret, std::vector<std::shared_ptr<VFXItem>>& items, int item_index,
        std::vector<std::shared_ptr<MeshSplit>>& splits);

    void AddMeshSplitsItem(std::vector<std::shared_ptr<MeshSplit>>& ret,
        std::vector<std::shared_ptr<VFXItem>>& items, std::shared_ptr<VFXItem> item, std::vector<std::shared_ptr<MeshSplit>>& splits);

public:
    CalculateGroup* calculate_group_ = nullptr;

    Composition* composition_ = nullptr;

    std::vector<std::shared_ptr<VFXItem>> items_;

    std::vector<std::shared_ptr<MarsMesh>> meshes_;

    float time_ = 0;

private:
#ifdef UNITTEST
public:
#endif
    std::vector<std::shared_ptr<MeshSplit>> mesh_splits_;

    std::vector<std::shared_ptr<VFXItem>> items_to_remove_;

    std::vector<std::shared_ptr<VFXItem>> items_to_add_;
};

}

#endif
