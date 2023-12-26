#ifndef MN_PLUGIN_SPRITE_SPRITE_MESH_H_
#define MN_PLUGIN_SPRITE_SPRITE_MESH_H_

#include <vector>
#include "ri/backend/opengl/gl_gpu_capability.hpp"
#include "math/transform.h"
#include "util/util.hpp"
#include "sprite_vfx_item.h"

#include "filters/filter_define.hpp"

// todo
#include "sceneData/mars_data_base.h"
#include "sceneData/data/sprite_mesh_geometry.hpp"

namespace mn {

struct SpriteItemRenderInfo;
class SpriteItem;
class MarsRenderer;
class MarsMaterial;
class MarsMesh;
class CalculateGroup;
class Shader;

struct SpriteRenderData {
    float life;
    std::shared_ptr<Transform> transform;
    Vec3* start_size = nullptr;
    Vec4* color = nullptr;
    Vec4* tex_offset = nullptr;
    bool hide = false;
    bool active = false;
    std::shared_ptr<SpriteMeshGeometry> geo_data = nullptr;

    ~SpriteRenderData() {
        MN_SAFE_DELETE(start_size);
        MN_SAFE_DELETE(color);
        MN_SAFE_DELETE(tex_offset);
    }
};

class SpriteMesh {
public:
    static void SetSpriteMeshMaxItemCountByGPU(std::shared_ptr<GLGPUCapability> gpu);

    static SpriteItemRenderInfo* GetImageItemRenderInfo(SpriteItem* item);

    static int GetMaxSpriteMeshItemCount();

    static std::string GetSpriteMeshShaderIdFromRenderInfo(SpriteItemRenderInfo* render_info, int count);

    // return ShaderCacheId
    static std::string SpriteMeshShaderFromRenderInfo(Shader& shader, SpriteItemRenderInfo* render_info, int count);

    static void SpriteMeshShaderFromFilter(Shader& shader, FilterDefine* filter, int count);
        
    SpriteMesh(SpriteItemRenderInfo* render_info, MarsRenderer* renderer, CalculateGroup* calculate_group);

    ~SpriteMesh();

    int GetItemCount() const;

    int GetAvailableItemCount() const;

    int GetListIndexStart() const;

    int GetListIndexEnd() const;

    void SetItems(std::vector<SpriteItem*> items);

    void Destroy();

    void UpdateItem(SpriteItem* item, bool init = false);

    void AddItem(SpriteItem* item);
    
    void RemoveItem(SpriteItem* item);

    void ApplyChange();
    
    std::shared_ptr<MarsMesh> GetInnerMesh() {
        return mesh_;
    }
    
    bool GetItemRenderData(SpriteVFXItemRenderData& ret, SpriteItem* item);
    
    bool GetItemRegionData(SpriteVFXItemRenderData& ret, SpriteItem* item);

private:
    void CreateMaterial(SpriteItemRenderInfo* render_info, int count, std::shared_ptr<MarsMesh> mesh);
    
    std::shared_ptr<SpriteMeshGeometry> GetItemInitData(SpriteItem* item, int idx, int point_start_index, int texture_index);

    SpriteMeshGeometry* GetItemGeometryData(SpriteItem* item, int a_index);

#ifdef UNITTEST
public:
#endif
    static void SetSpriteMeshMaxItemCount(int count);

public:
    std::shared_ptr<MarsMesh> mesh_;
    
    std::vector<SpriteItem*> items_;
    
    int pre_multi_alpha_ = 0;
    
    int mtl_slot_count_ = 0;
    
    bool is_dirty_ = false;
    
    int max_item_count_ = 0;
    
    bool split_layer_ = false;

private:
    CalculateGroup* calculate_group_ = nullptr;

};

}

#endif
