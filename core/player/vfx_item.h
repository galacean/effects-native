#ifndef MN_PLAYER_VFX_ITEM_H_
#define MN_PLAYER_VFX_ITEM_H_

#include <string>
#include <vector>
#include "sceneData/mars_data_base.h"
#include "sceneData/scene_data_decoder.h"
#include "math/transform.h"
#include "util/constant.hpp"

namespace mn {

class Composition;
class BaseContentItem;
class GLShaderLibrary;

// todo: refactor，VFXItem暂时用shared_ptr存，后面理清销毁时机再改

class VFXItem {
public:
    VFXItem();

    virtual ~VFXItem();
    
    void Create(ItemData* arg, Composition* comp, bool is_comp = false);
    
    // todo: change to enum
    virtual std::string GetType() const {
        return VFX_ITEM_TYPE_BASE;
    }
    
    virtual void OnConstructed(ItemData* arg) {}

    void Start();

    void Reset();

    virtual void InnerOnUpdate(float dt, std::shared_ptr<VFXItem> thiz);

    virtual bool GetContentVisible() {
        return content_visible_ && !hide_;
    }

    void* GetContent() {
        return content_;
    }
    
    virtual BaseContentItem* CreateContent() {
        if (!content_) {
            content_ = InnerCreateContent();
        }
        return content_;
    }

    virtual void Precompile(GLShaderLibrary* shader_library) {}

    void Stop();
    
    void Destroy(std::shared_ptr<VFXItem> thiz);
    
    // filter需要继承
    virtual const char* GetCachePrefix() { return nullptr; }
    
    // 目前之后calculate和tree节点实现
    virtual void OnUpdateByLoader(float dt, std::shared_ptr<VFXItem> thiz) {}
    
    bool GetLifetimeStarted() {
        return started_ && (!delaying_);
    }
    
protected:
    virtual BaseContentItem* InnerCreateContent() { return nullptr; }

    virtual void InnerStop() {}

    virtual void OnItemRemoved(Composition* renderer, void* content) {}

    virtual void OnLifeTimeBegin(Composition* renderer, void* content) {}
    
    virtual void OnItemUpdate(float dt, float lifetime) {}

    virtual void OnEnd() {}

    virtual bool IsEnded(float now) {
        return now - dur_in_ms_ > 0.001f;
    }

public:
    // todo: refactor
    std::shared_ptr<Transform> transform_;

    float duration_ = 0;

    bool reusable_ = false;

    bool looping_ = false;

    float lifetime_ = 0;

    Composition* composition_ = nullptr;

    std::string parent_id_;

    float delay_ = 0;

    END_BEHAVIOR_TYPE end_behavior_ = END_BEHAVIOR_DESTROY;

    int list_index_ = 0;

    std::string name_;

    std::string id_;

    bool ended_ = false;

    float v_priority_ = 0;

//protected:
    BaseContentItem* content_ = nullptr;

    bool hide_ = false;

    bool frozen_ = false;

    bool content_visible_ = false;

    float time_in_ms_ = 0;

    float delay_in_ms_ = 0;

    float dur_in_ms_ = 0;

    bool started_ = false;

    bool delaying_ = false;

    bool call_end_ = false;
    
    bool skip_update_ = false;
    
    // for tree item
    std::shared_ptr<VFXItem> parent;
};

}

#endif
