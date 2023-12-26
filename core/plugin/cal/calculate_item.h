#ifndef MN_PLUGIN_CAL_CALCULATE_ITEM_H_
#define MN_PLUGIN_CAL_CALCULATE_ITEM_H_

#include "plugin/types.h"
#include "plugin/sprite/sprite_item.h"
#include "plugin/sprite/sprite_mesh.h"

namespace mn {

class CalculateGroup;
class VFXItem;

struct CalculateOptions {
    int id;
    int parent_id;
    int ref_count;
    bool reusable;
    bool relative;
};

class CalculateItem : public SpriteItem {
public:
    CalculateItem(SpriteContentData* props, MeshRendererOptions* opt, VFXItem* vfx_item);

    ~CalculateItem();

    void OnUpdate(float dt, CalculateGroup* group);
    
protected:
    int InitTexture(int texture, MeshRendererOptions* options) override;
    
    bool WillTranslate() override;

private:
    SpriteRenderData* GetRenderData(float time, bool init = false) override;

public:
    SpriteRenderData* render_data_ = nullptr;
    int ref_count_ = 0;
    std::string id_;
    bool active_ = false;
    
    Transform* custom_transform_ = nullptr;
};

}

#endif
