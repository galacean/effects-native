//
//  ar_track_vfx_item.h
//  MarsNative
//
//  Created by changxing on 2022/9/19.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef ar_track_vfx_item_h
#define ar_track_vfx_item_h

#include "player/vfx_item.h"
#include "include/mars_native_ar_track_plugin.h"
#include "sceneData/scene_data_decoder.h"

namespace mn {

class ARTrackVfxItem : public VFXItem {
public:
    ARTrackVfxItem(ItemData* options, Composition* composition);

    ~ARTrackVfxItem();

    std::string GetType() const override {
        return "AR_TRACK";
    }

    void OnConstructed(ItemData* arg) override;

    void InnerOnUpdate(float dt, std::shared_ptr<VFXItem> thiz) override;
    
    void UpdateTrackInfo(MNARTrackInfo* info);
    
protected:
    BaseContentItem* InnerCreateContent() override;
    
    void OnLifeTimeBegin(Composition* renderer, void* content) override;
    
private:
    void UpdateARTrackItems();
    
    void GetTransformOffset(Mat4& out, int p0, int p1, int p2);
    
private:
    MNARTrackInfo origin_track_info_;
    MNARTrackInfo curr_track_info_;
    
    int track_items_count_ = 0;
    PluginARContentOptions2DItemData* track_items_ref_ = nullptr;
    
    bool valid_ = true;
    bool visible_ = false;
};

}

#endif /* ar_track_vfx_item_h */
