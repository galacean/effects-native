#include "vfx_value_getter.h"
#include "math/value/static_value_getter.hpp"
#include "math/value/path_value_getter.hpp"
#include "math/value/linear_value_getter.hpp"
#include "math/value/line_segments_value_getter.hpp"
#include "math/value/curve_value_getter.hpp"
#include "math/value/bezier_value_getter.hpp"
#include "math/value/random_value_getter.hpp"
#include "math/value/gradient_value_getter.hpp"
#include "util/log_util.hpp"

namespace mn {

static const size_t CURVE_ROW_LENGTH = 4;
static const size_t LINE_ROW_LENGTH = 2;
static const size_t PATH_KEY_ROW_LENGTH = 4;
static const size_t PATH_VALUE_ROW_LENGTH = 3;
static const size_t CAP_VALUE_ROW_LENGTH = 3;

ValueGetter* VFXValueGetter::CreateConstantValueGetter(float data) {
    ValueGetter *valueGetter = new StaticValueGetter(data);
    return valueGetter;
}

ValueGetter* VFXValueGetter::CreateValueGetter(MNumberOrValue* any_value) {
    if (!any_value) {
        return CreateConstantValueGetter(0);
    }
    if (any_value->data_type_ == MDataType::NUMBER) {
        return CreateConstantValueGetter(any_value->number_);
    } else {
        return CreateObjectValueGetter(any_value->value_.get());
    }
}

ValueGetter* VFXValueGetter::CreateObjectValueGetter(MValue* value) {
    std::string value_type = value->type_;
    int array_length = value->length_;
    float* data = value->data_;
    if (value_type.compare("curve") == 0) {
        int length = array_length / CURVE_ROW_LENGTH;
        auto curve_value_data = new float[length][CURVE_ROW_LENGTH]();
        for (size_t i = 0; i < length; i++) {
            for (size_t j = 0; j < CURVE_ROW_LENGTH; j++) {
                curve_value_data[i][j] = data[i * CURVE_ROW_LENGTH + j];
            }
        }
        return new CurveValueGetter(curve_value_data, length);
    } else if (value_type.compare("lines") == 0) {
        int length = array_length / LINE_ROW_LENGTH;
        auto lines_value_data = new float[length][LINE_ROW_LENGTH]();
        for (size_t i = 0; i < length; i++) {
            for (size_t j = 0; j < LINE_ROW_LENGTH; j++) {
                lines_value_data[i][j] = data[i * LINE_ROW_LENGTH + j];
            }
        }
        if (length == 2 && lines_value_data[0][0] == 0 && lines_value_data[1][0] == 1) {
            auto ret = new LinearValueGetter(lines_value_data[0][1], lines_value_data[1][1]);
            delete[] lines_value_data;
            return ret;
        }
        return new LineSegmentsValueGetter(lines_value_data, length);
    } else if (value_type.compare("path") == 0) {
        int length = array_length / (PATH_KEY_ROW_LENGTH + PATH_VALUE_ROW_LENGTH);
        auto key_data = new float[length][PATH_KEY_ROW_LENGTH];
        for (size_t i = 0; i < length; i++) {
            for (size_t j = 0; j < PATH_KEY_ROW_LENGTH; j++) {
                key_data[i][j] = data[i * PATH_KEY_ROW_LENGTH + j];
            }
        }

        auto value_data = new float[length][PATH_VALUE_ROW_LENGTH];
        for (size_t i = 0; i < length; i++) {
            for (size_t j = 0; j < PATH_VALUE_ROW_LENGTH; j++) {
                value_data[i][j] = data[length * PATH_KEY_ROW_LENGTH + i * PATH_VALUE_ROW_LENGTH + j];
            }
        }
        return new PathValueGetter(key_data, value_data, length);
    } else if (value_type.compare("bezier") == 0) {
        int length = array_length / (PATH_KEY_ROW_LENGTH + PATH_VALUE_ROW_LENGTH);
        int length2 = value->length2_ / CAP_VALUE_ROW_LENGTH;
        size_t keyLength = length;
        size_t capLength = length2;

        auto key_data = new float[length][PATH_KEY_ROW_LENGTH];
        for (size_t i = 0; i < length; i++) {
            for (size_t j = 0; j < PATH_KEY_ROW_LENGTH; j++) {
                key_data[i][j] = data[i * PATH_KEY_ROW_LENGTH + j];
            }
        }

        auto value_data = new float[length][PATH_VALUE_ROW_LENGTH];
        for (size_t i = 0; i < length; i++) {
            for (size_t j = 0; j < PATH_VALUE_ROW_LENGTH; j++) {
                value_data[i][j] = data[length * PATH_KEY_ROW_LENGTH + i * PATH_VALUE_ROW_LENGTH + j];
            }
        }

        auto cap_data = new float[capLength][CAP_VALUE_ROW_LENGTH];
        for (size_t i = 0; i < capLength; i++) {
            for (size_t j = 0; j < CAP_VALUE_ROW_LENGTH; j++) {
                cap_data[i][j] = value->data2_[i * CAP_VALUE_ROW_LENGTH + j];
            }
        }

        return new BezierValueGetter(key_data, value_data, cap_data, keyLength, capLength);
    } else if (value_type.compare("random") == 0) {
        return new RandomValueGetter(data[0], data[1]);
    } else if (value_type.compare("gradient") == 0) {
        GradientValueGetter* ret = new GradientValueGetter();
        for (int i = 0; i < array_length; i += 5) {
            ColorStop color_stop;
            color_stop.stop = data[i];
            color_stop.SetColor(data[i + 1] * 255.0f, data[i + 2] * 255.0f, data[i + 3] * 255.0f, data[i + 4] * 255.0f);
            ret->AddColorStop(color_stop);
        }
        return ret;
    } else {
        MLOGE("invalid type %s", value_type.c_str());
        assert(0);
    }

    return nullptr;
}

ValueGetter* VFXValueGetter::CreateColorValueGetter(MValue* rawData) {
    float color[4] = {1, 1, 1, 1};

    if (rawData) {
        const std::string& type = rawData->type_;

        if (type.compare("color") == 0) {
            color[0] = rawData->data_[0];
            color[1] = rawData->data_[1];
            color[2] = rawData->data_[2];
            color[3] = rawData->data_[3];
            ValueGetter* valueGetter = new StaticValueGetter(color, 4);
            return valueGetter;
        } else if (type.compare("colors") == 0) {
            RandomSetValue *valueGetter = new RandomSetValue();
            for (int i = 0; i < rawData->length_; i += 4) {
                float r = rawData->data_[i];
                float g = rawData->data_[i + 1];
                float b = rawData->data_[i + 2];
                float a = rawData->data_[i + 3];
                valueGetter->AddColor(r, g, b, a);
            }
            return valueGetter;
        } else if (type.compare("gradient") == 0) {
            GradientValueGetter* ret = new GradientValueGetter();
            for (int i = 0; i < rawData->length_; i += 5) {
                ColorStop color_stop;
                color_stop.stop = rawData->data_[i];
                color_stop.SetColor(
                    rawData->data_[i + 1],
                    rawData->data_[i + 2],
                    rawData->data_[i + 3],
                    rawData->data_[i + 4]
                );
                ret->AddColorStop(color_stop);
            }
            return ret;
        }
    } else {
        MLOGE("create ColorValueGetter failed");
        assert(0);
    }

    ValueGetter *defaultValueGetter = new StaticValueGetter(color, 4);
    return defaultValueGetter;
}

bool VFXValueGetter::CreateGradientColor(std::vector<ColorStop>& out_color, std::shared_ptr<MValue> value) {
    std::string value_type = value->type_;
    int array_length = value->length_;
    float* data = value->data_;
    
    if (value_type.compare("gradient") != 0) {
        return false;
    }
    
    out_color.clear();
    for (int i = 0; i < array_length; i += 5) {
        ColorStop color_stop;
        color_stop.stop = data[i];
        color_stop.SetColor(data[i + 1], data[i + 2], data[i + 3], data[i + 4]);
        out_color.push_back(color_stop);
    }
    return true;
}

}
