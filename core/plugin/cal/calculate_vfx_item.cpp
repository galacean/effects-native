#include "calculate_vfx_item.h"
#include "player/composition.h"

namespace mn {

CalculateVFXItem::CalculateVFXItem(ItemData* options, Composition* composition) {
    v_priority_ = 1;
    Create(options, composition);
}

CalculateVFXItem::~CalculateVFXItem() {
}

void CalculateVFXItem::OnConstructed(ItemData* arg) {
    cal_ = (SpriteContentData*) arg->content;
    // todo: relative?
}

void CalculateVFXItem::InnerOnUpdate(float dt, std::shared_ptr<VFXItem> thiz) {
}

void CalculateVFXItem::OnUpdateByLoader(float dt, std::shared_ptr<VFXItem> thiz) {
    VFXItem::InnerOnUpdate(dt, thiz);
}

BaseContentItem* CalculateVFXItem::InnerCreateContent() {
    MeshRendererOptions* opt = composition_->GetParticleOptions();
    return new CalculateItem(cal_, opt, this);
}

void CalculateVFXItem::OnLifeTimeBegin(Composition* renderer, void* content) {
    ((CalculateItem*) content)->active_ = true;
}

SpriteRenderData* CalculateVFXItem::GetCurrentRenderData() {
    if (content_) {
        return ((CalculateItem*) content_)->render_data_;
    }
    return nullptr;
}

}
