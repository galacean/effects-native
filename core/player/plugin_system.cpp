#include "plugin_system.h"
#include "plugin/cal/calculate_loader.h"
#include "plugin/sprite/sprite_loader.h"
#include "plugin/particle/particle_loader.hpp"
#include "plugin/interact/interact_loader.h"
#include "plugin/arTrack/ar_track_loader.h"
#include "model/model_plugin.hpp"
#include "model/model_tree_plugin.h"
#include "util/log_util.hpp"

namespace mn {

PluginSystem::PluginSystem() {
    MLOGD("PluginSystem %p", this);
    RegisterPlugin("cal", new CalPluginBuilder, true);
    RegisterPlugin("sprite", new SpritePluginBuilder, true);
    RegisterPlugin("particle", new ParticlePluginBuilder, true);
    RegisterPlugin("interact", new InteractPluginBuilder, true);
    RegisterPlugin("model", new ModelPluginBuilder, true/*todo: 临时写成true*/);
    RegisterPlugin("tree", new ModelTreePluginBuilder, true/*todo: 临时写成true*/);
}

PluginSystem::~PluginSystem() {
    MLOGD("~PluginSystem %p", this);
    for (int i = 0; i < loaders_.size(); i++) {
        delete loaders_[i];
    }
    for (auto iter = plugin_builder_map_.begin(); iter != plugin_builder_map_.end(); iter++) {
        delete iter->second;
    }
}

void PluginSystem::RegisterPlugin(const std::string& name, MarsPluginBuilder* loader_builder, bool is_default) {
    plugin_builder_map_[name] = loader_builder;
    if (is_default) {
        default_plugins_.push_back(name);
    }
}

bool PluginSystem::RegisterNativePlugin(const std::string& name, MarsNativePluginBuilder* native_loader_builder) {
    if (name.compare(PLUGIN_AR_TRACK) == 0) {
        RegisterPlugin(name, new ARTrackPluginBuilder(native_loader_builder), false);
    } else {
        return false;
    }
    return true;
}

// const std::vector<std::string>& PluginSystem::GetDefaultPlugins() {
//     return default_plugins_;
// }

//MarsPlugin* PluginSystem::GetPluginLoader(const std::string& name) {
//    auto iter = plugin_builder_map_.find(name);
//    if (iter == plugin_builder_map_.end()) {
//        MLOGE("plugin %s no registered loader", name.c_str());
//        assert(0);
//    }
//    return iter->second->CreatePlugin();
//}

void PluginSystem::InitPlugins() {
    for (auto iter : plugin_builder_map_) {
        MarsPlugin* plugin = iter.second->CreatePlugin();
        loaders_.push_back(plugin);
    }
    
    if (loaders_.size()) {
        for (int i = 0; i < loaders_.size() - 1; i++) {
            for (int j = i + 1; j < loaders_.size(); j++) {
                if (loaders_[j]->order_ < loaders_[i]->order_) {
                    auto temp = loaders_[i];
                    loaders_[i] = loaders_[j];
                    loaders_[j] = temp;
                }
            }
        }
    }
    
    
    
}

void PluginSystem::InitializeComposition(Composition* comp, SceneDataDecoder* scene) {
    for (int i = 0; i < loaders_.size(); i++) {
        loaders_[i]->OnCompositionConstructed(comp, scene);
    }
}

void PluginSystem::ResetComposition(Composition* comp, MarsPlayerRenderFrame* render_frame) {
    for (int i = 0; i < loaders_.size(); i++) {
        loaders_[i]->OnCompositionReset(comp, render_frame);
    }
}

void PluginSystem::DestroyComposition(Composition* comp) {
    for (int i = 0; i < loaders_.size(); i++) {
        loaders_[i]->OnCompositionDestroyed(comp);
    }
}

std::shared_ptr<VFXItem> PluginSystem::CreatePluginItem(const std::string& type, ItemData* options, Composition* composition) {
    std::string temp = type;
    if (type.compare("plugin") == 0) {
        int plugin_type = ((PluginContentData*) options->content)->options->type;
        if (plugin_type == 1) {
            temp = "arTrack";
        }
    }
    auto iter = plugin_builder_map_.find(temp);
    if (iter == plugin_builder_map_.end()) {
        MLOGD("ERROR: plugin %s no registered constructor", temp.c_str());
        assert(0);
        return nullptr;
    }
    
    auto item = iter->second->CreateItem(options, composition);
    return item;
}

}
