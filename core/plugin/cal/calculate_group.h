#ifndef MN_PLUGIN_CAL_CALCULATE_GROUP_H_
#define MN_PLUGIN_CAL_CALCULATE_GROUP_H_

#include <map>
#include "calculate_item.h"

namespace mn {

class CGNode {
public:
    CGNode(CalculateItem* content);

    ~CGNode();

    static void RemoveContent(std::vector<std::shared_ptr<CGNode>>& nodes, CalculateItem* content);

public:
    CalculateItem* content_ = nullptr;
    std::weak_ptr<ObjectLifeChecker> content_life_checker_;

    std::vector<std::shared_ptr<CGNode>> children_;
};

class CalculateGroup {
public:
    static SpriteRenderData* CombineRenderData(SpriteRenderData* data, SpriteRenderData* parent_data);

    CalculateGroup();
    
    ~CalculateGroup();

    void OnUpdate(float dt);

    SpriteRenderData* GetRenderData(std::string id);

    void AddItem(CalculateItem* item);

    void RemoveItemRef(const std::string& id);

    void BuildTree(const std::map<std::string, int>& ref_count_map);

private:
    std::string InnerRemoveItemRef(const std::string& id);

public:
    std::vector<std::shared_ptr<CGNode>> nodes_;
    std::vector<CalculateItem*> items_;
    std::map<std::string, CalculateItem*> item_map_;
    std::map<std::string, Transform*> world_transform_map_;
    float time_ = 0;
};

}

#endif
