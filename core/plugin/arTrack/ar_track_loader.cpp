//
//  ar_track_loader.cpp
//  MarsNative
//
//  Created by changxing on 2022/9/19.
//  Copyright © 2022 Alipay. All rights reserved.
//

#include "ar_track_loader.h"

namespace mn {

ARTrackLoader::ARTrackLoader(MarsNativeARTrackPlugin* native_plugin) : native_plugin_(native_plugin) {
    order_ = -1;
}

ARTrackLoader::~ARTrackLoader() {
}

void ARTrackLoader::OnCompositionWillUpdateItems(float dt) {
    if (native_plugin_) {
        MNARTrackInfo* info = native_plugin_->GetCurrentTrackInfo(dt);
        auto item = item_.lock();
        if (item) {
            item->UpdateTrackInfo(info);
        }
    }
}

}
