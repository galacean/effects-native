//
//  ar_track_loader.h
//  PlayGroundIOS
//
//  Created by changxing on 2022/9/15.
//

#ifndef ar_track_loader_h
#define ar_track_loader_h

#include "player/plugin_system.h"
#include "include/mars_native_ar_track_plugin.h"
#include "ar_track_vfx_item.h"

namespace mn {

class ARTrackLoader : public MarsPlugin {
public:
    ARTrackLoader(MarsNativeARTrackPlugin* native_plugin);

    ~ARTrackLoader();
    
    void OnCompositionWillUpdateItems(float dt) override;
    
    void SetVFXItem(std::shared_ptr<ARTrackVfxItem> item) {
        item_ = item;
    }

private:
    MarsNativeARTrackPlugin* native_plugin_ = nullptr;
    
    std::weak_ptr<ARTrackVfxItem> item_;
};

class ARTrackPluginBuilder : public MarsPluginBuilder {
public:
    ARTrackPluginBuilder(MarsNativePluginBuilder* native_builder) {
        native_builder_ = native_builder;
    }

    MarsPlugin* CreatePlugin() override {
        // 每个动画只有一个ar插件
        if (!ar_track_) {
            ar_track_ = new ARTrackLoader((MarsNativeARTrackPlugin*) native_builder_->CreateNew());
        }
        return ar_track_;
    }

    std::shared_ptr<VFXItem> CreateItem(ItemData* options, Composition* composition) override {
        auto ret = std::make_shared<ARTrackVfxItem>(options, composition);
        ar_track_->SetVFXItem(ret);
        return ret;
    }

private:
    ARTrackLoader* ar_track_ = nullptr;
    
    MarsNativePluginBuilder* native_builder_;
};

}

#endif /* ar_track_loader_h */
