#include "calculate_item.h"
#include "calculate_group.h"
#include "util/log_util.hpp"
#include "calculate_vfx_item.h"
#include "player/composition.h"
#include "player/plugin_system.h"
#include "plugin/arTrack/ar_track_loader.h"

namespace mn {

CalculateItem::CalculateItem(SpriteContentData* props, MeshRendererOptions* opt, VFXItem* vfx_item)
        : SpriteItem(props, opt, vfx_item) {
    render_data_ = GetRenderData(0, true);
    id_ = vfx_item->id_;
    active_ = false;
}

CalculateItem::~CalculateItem() {
    MN_SAFE_DELETE(render_data_);
    MN_SAFE_DELETE(custom_transform_);
}

void CalculateItem::OnUpdate(float dt, CalculateGroup* group) {
    UpdateTime(group->time_);
    SpriteRenderData* data = GetRenderData(time_, true);
    SpriteRenderData* parent_data = nullptr;
    if (!parent_id_.empty()) {
        parent_data = group->GetRenderData(parent_id_.c_str());
    }
    data = CalculateGroup::CombineRenderData(data, parent_data);
    MN_SAFE_DELETE(render_data_);
    render_data_ = data;
}

int CalculateItem::InitTexture(int texture, MeshRendererOptions* options) {
    return texture;
}

bool CalculateItem::WillTranslate() {
    return true;
}

SpriteRenderData* CalculateItem::GetRenderData(float time, bool init) {
    SpriteRenderData* ret = SpriteItem::GetRenderData(time, init);
    if (ret->start_size) {
        const auto& scaling = ret->transform->scale_;
        ret->transform->SetScale(
            scaling->m[0] * ret->start_size->m[0],
            scaling->m[1] * ret->start_size->m[1],
            scaling->m[2] * ret->start_size->m[2]
        );
        MN_SAFE_DELETE(ret->start_size);
    }
    ret->active = active_;
    if (custom_transform_) {
        ret->transform->Mul(custom_transform_);
    }
    return ret;
}

}
