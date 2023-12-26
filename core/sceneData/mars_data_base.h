#ifndef MN_MARS_DATA_BASE_H_
#define MN_MARS_DATA_BASE_H_

#include <string>
#include <vector>
#include <sstream>

#ifdef DEBUG
//#define PRINT_MN_DATA
#endif

#ifdef PRINT_MN_DATA
#define PRINT_DATA_INT(tag, value) MLOGD("MNDATA: %s %d", tag, (int) value)
#define PRINT_DATA_FLOAT(tag, value) MLOGD("MNDATA: %s %f", tag, (float) value)
#define PRINT_DATA_STRING(tag, value) MLOGD("MNDATA: %s (%s)", tag, value)
#define PRINT_DATA_INT_ARRAY(tag, value, len) { std::stringstream ss; for (int i = 0; i < len; i++) {ss << value[i] << ","; } MLOGD("MNDATA: %s %s", tag, ss.str().data()); }
#define PRINT_DATA_FLOAT_ARRAY(tag, value, len) { std::stringstream ss; for (int i = 0; i < len; i++) {ss << value[i] << ","; } MLOGD("MNDATA: %s %s", tag, ss.str().data()); }
#define PRINT_DATA_VALUE(tag, value) PRINT_DATA_FLOAT_ARRAY(tag, value->data_, value->length_); if (value->data2_) PRINT_DATA_FLOAT_ARRAY("\tpart2", value->data2_, value->length2_);
#define PRINT_DATA_NUMBER_EXP(tag, value) { if (value->data_type_ == MDataType::NUMBER) { PRINT_DATA_FLOAT(tag, value->number_); } else { PRINT_DATA_VALUE(tag, value->value_); } };
#else
#define PRINT_DATA_INT(tag, value)
#define PRINT_DATA_FLOAT(tag, value)
#define PRINT_DATA_STRING(tag, value)
#define PRINT_DATA_INT_ARRAY(tag, value, len)
#define PRINT_DATA_FLOAT_ARRAY(tag, value, len)
#define PRINT_DATA_VALUE(tag, value)
#define PRINT_DATA_NUMBER_EXP(tag, value)
#endif

#ifdef PRINT_MN_DATA
#define CATCH_EXCEPTION(name) {PRINT_DATA_INT("invalid "#name" type", tag.val);delete data;assert(0);return nullptr;}
#define catch_data_exception(name) {PRINT_DATA_INT("invalid "#name" type", tag.val);assert(0);return nullptr;}
#else
#define CATCH_EXCEPTION(name) {MLOGE("invalid "#name" type", tag.val);delete data;return nullptr;}
#define catch_data_exception(name) {MLOGE("invalid "#name" type", tag.val);assert(0);return nullptr;}
#endif

#define TYPE_INT 1
#define TYPE_FLOAT 2
#define TYPE_STRING 3
#define TYPE_FLOATARRAY 4
#define TYPE_VALUE 5
#define TYPE_NUMBER_EXP 6
#define TYPE_VEC3_EXP 7
#define TYPE_COLOR_EXP 8
#define TYPE_INTARRAY 9
#define TYPE_OBJECT 100

#define data_type_is_string (tag.i.type == TYPE_STRING)
#define data_type_is_float (tag.i.type == TYPE_FLOAT)
#define data_type_is_int (tag.i.type == TYPE_INT)
#define data_type_is_object (tag.i.type == TYPE_OBJECT)
#define data_type_is_floatarray (tag.i.type == TYPE_FLOATARRAY)
#define data_type_is_numbder_exp (tag.i.type == TYPE_NUMBER_EXP)
#define data_type_is_vec3_exp (tag.i.type == TYPE_VEC3_EXP)
#define data_type_is_color_exp (tag.i.type == TYPE_COLOR_EXP)
#define data_type_is_intarray (tag.i.type == TYPE_INTARRAY)

#define MODEL_TYPE_Int8Array 1
#define MODEL_TYPE_Uint8Array 2
#define MODEL_TYPE_Uint8ClampedArray 3
#define MODEL_TYPE_Int16Array 4
#define MODEL_TYPE_Uint16Array 5
#define MODEL_TYPE_Int32Array 6
#define MODEL_TYPE_Uint32Array 7
#define MODEL_TYPE_Float32Array 8
#define MODEL_TYPE_Float64Array 9
#define MODEL_TYPE_BinaryPointer 10

namespace mn {

enum class MDataType {
    NUMBER = 1,
    ARRAY = 2,
};

union DataTag {
    int32_t val;
    struct {
        uint16_t idx;
        uint16_t type;
    } i;
    
};

class MValue {
public:
    std::string type_;
    float* data_;
    float* data2_ = nullptr;
    int length_;
    int length2_;
};

struct MBool {
    MBool(bool val) {
        this->val = val;
    }

    bool val;
};

struct MInt {
    MInt(int val) {
        this->val = val;
    }

    int val;
};

struct MUint {
    MUint(uint32_t val) {
        this->val = val;
    }

    uint32_t val;
};

struct MFloat {
    MFloat(float val) {
        this->val = val;
    }

    float val;
};

class MNumberOrValue {
public:
    MNumberOrValue(float number) {
        data_type_ = MDataType::NUMBER;
        number_ = number;
    }

    MNumberOrValue(std::shared_ptr<MValue> value) {
        data_type_ = MDataType::ARRAY;
        value_ = value;
    }
    
    bool IsZero() {
        return ((data_type_ == MDataType::NUMBER) && (number_ == 0));
    }

    MDataType data_type_;
    float number_;
    std::shared_ptr<MValue> value_;
};

struct MarsBinData {
    int length = 0;
    uint8_t* data = nullptr;
};

class MBuffer {
public:
    MBuffer(uint8_t* buffer);

    ~MBuffer();

    int ReadInt();
    
    uint32_t ReadUint32();

    float ReadFloat();

    const char* ReadString();
    
    int* ReadIntArray(int length);

    float* ReadFloatArray(int length);
    
    uint8_t* ReadUint8Array(int byte_len);

    std::shared_ptr<MValue> ReadValue();
    
    void* ReadTypedArray(uint32_t& out_len, uint32_t& out_type);
    
    int CurrentIdx() const {
        return idx_;
    }

public:
    std::vector<MarsBinData> bins_;
    
private:
    uint8_t* buffer_;
    int* int_buffer_;
    uint32_t* uint32_buffer_;
    float* float_buffer_;

    int idx_ = 0;
};

class MImageData {
public:
    MImageData(const std::string& file_path) {
        file_path_ = file_path;
    }

    ~MImageData() {
    }

    uint8_t* GetKtxImage();
    
    std::string file_path_;
};

} // namespace mn

#endif
