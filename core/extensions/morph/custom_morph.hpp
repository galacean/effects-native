//
//  custom_morph.hpp
//  MarsNative
//
//  Created by changxing on 2023/9/14.
//  Copyright © 2023 Alipay. All rights reserved.
//

#ifndef custom_morph_hpp
#define custom_morph_hpp

#include <string>
#include <vector>
#include <functional>
#include "player/plugin_system.h"

#define MN_CUSTOM_MORPH_CALLBACK std::function<void(const std::string& name, float time, float** points, int* points_length, uint16_t** indices, int* indices_length)>

namespace mn {

class SpriteMeshGeometry;

class CustomMorph {
public:
    CustomMorph(const std::string& name, const MN_CUSTOM_MORPH_CALLBACK& callback);
    
    ~CustomMorph();
    
    std::shared_ptr<SpriteMeshGeometry> Generate(float lifetime);
    
private:
    std::string name_;
    MN_CUSTOM_MORPH_CALLBACK callback_;
};

class CustomMorphLoader : public MarsPlugin {
public:
    CustomMorphLoader(const std::vector<std::string>& params, const MN_CUSTOM_MORPH_CALLBACK& callback);

    ~CustomMorphLoader();
    
    void OnCompositionItemLifeBegin(Composition* comp, std::shared_ptr<VFXItem> item) override;
    
private:
    std::vector<std::string> params_;
    MN_CUSTOM_MORPH_CALLBACK callback_;
};

class CustomMorphPluginBuilder : public MarsPluginBuilder {
public:
    CustomMorphPluginBuilder(const std::vector<std::string>& params, const MN_CUSTOM_MORPH_CALLBACK& callback) {
        params_ = params;
        callback_ = callback;
    }
    
    MarsPlugin* CreatePlugin() override {
        return new CustomMorphLoader(params_, callback_);
    }

    std::shared_ptr<VFXItem> CreateItem(ItemData* options, Composition* composition) override {
        return nullptr;
    }
    
private:
    std::vector<std::string> params_;
    MN_CUSTOM_MORPH_CALLBACK callback_;
};

}

#endif /* custom_morph_hpp */
