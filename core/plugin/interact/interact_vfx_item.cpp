#include "interact_vfx_item.h"
#include "player/composition.h"

namespace mn {

InteractVFXItem::InteractVFXItem(ItemData* options, Composition* composition) {
    Create(options, composition);
}

InteractVFXItem::~InteractVFXItem() {
}

void InteractVFXItem::OnConstructed(ItemData* arg) {
    interact_ = (InteractContentData*) arg->content;
}

BaseContentItem* InteractVFXItem::InnerCreateContent() {
    return nullptr;
}

void InteractVFXItem::OnLifeTimeBegin(Composition* renderer, void* content) {
    const auto& options = interact_->options;
    if (options->type == INTERACT_TYPE_MESSAGE) {
        renderer->OnMessageItem(name_, "MESSAGE_ITEM_PHRASE_BEGIN", id_);
    }
}

void InteractVFXItem::OnItemRemoved(Composition* renderer, void* content) {
    const auto& options = interact_->options;
    if (options->type == INTERACT_TYPE_MESSAGE) {
        renderer->OnMessageItem(name_, "MESSAGE_ITEM_PHRASE_END", id_);
    }
}

}
