#ifndef MN_VFX_VALUE_GETTER_H_
#define MN_VFX_VALUE_GETTER_H_

#include <string>
#include "math/value/value_getter.hpp"
#include "plugin/types.h"
#include "sceneData/scene_data_decoder.h"

namespace mn {

class VFXValueGetter {
public:
    static ValueGetter* CreateConstantValueGetter(float data);
    
    static ValueGetter* CreateValueGetter(MNumberOrValue* value);
    
    static ValueGetter* CreateObjectValueGetter(MValue* any_value);

    static ValueGetter* CreateColorValueGetter(MValue* rawData);
    
    static bool CreateGradientColor(std::vector<ColorStop>& out_color, std::shared_ptr<MValue> value);
};

}

#endif
