//
//  util.hpp
//
//  Created by Zongming Liu on 2021/12/7.
//

#ifndef util_hpp
#define util_hpp

#include <stdio.h>
#include <vector>
#include <cassert>

#include "ri/render_base.hpp"

#define MN_SAFE_DELETE(ptr) { if (ptr) { delete ptr; ptr = nullptr; } }

#define MN_SAFE_DELETE_ARR(ptr) { if (ptr) { delete [] ptr; ptr = nullptr; } }

namespace mn {

class Utils {
    
public:
    
    static int64_t TimestampMs();
    
    static unsigned char* Base64Decode(const char* data, size_t input_length, size_t* output_length);
    
    template <typename T> static int GetVectorIndexOfItem(std::vector<T>& arr, T& item) {
        for (int i = 0; i < arr.size(); i++) {
            if (arr[i] == item) {
                return i;
            }
        }
        return -1;
    }
    
    template <typename T> static void VectorSplice(std::vector<T>* out, std::vector<T>& arr, int index, int len) {
        if (out) {
            out->clear();
        }
        int temp_idx = 0;
        for (auto iter = arr.begin(); iter != arr.end(); temp_idx++) {
            if (temp_idx >= index && temp_idx < index + len) {
                if (out) {
                    out->push_back(*iter);
                }
                iter = arr.erase(iter);
            } else {
                iter++;
            }
        }
    }
    
    template <typename T> static void VectorAdd(std::vector<T>& arr, T item) {
        for (size_t i = 0; i < arr.size(); i++) {
            if (arr[i] == item) {
                return;
            }
        }
        arr.push_back(item);
    }
    
    template <typename T> static void VectorRemove(std::vector<T>& arr, T item) {
        for (auto iter = arr.begin(); iter != arr.end(); iter++) {
            if (*iter == item) {
                arr.erase(iter);
                return;
            }
        }
    }
    
    template <typename T> static int VectorFindIndex(std::vector<T>& arr, T& item) {
        for (auto iter = arr.begin(); iter != arr.end(); iter++) {
            if (*iter == item) {
                return iter - arr.begin();
            }
        }
        return -1;
    }
    
    template <typename T> static bool VectorInOfRange(std::vector<T>& arr, int index) {
        int size = arr.size();
        return index >= 0 && index < size;
    }
    
    // todo: cr 好好check下;
    static BufferDataType ConvertDecoderTypeToBufferDataType(int value) {
        if (value == 1) {
            return BufferDataType::BYTE;
        } else if (value == 2) {
            return BufferDataType::UNSIGNED_BYTE;
        } else if (value == 3) {
            assert(false);
        } else if (value == 4) {
            return BufferDataType::SHORT;
        } else if (value == 5) {
            return BufferDataType::UNSIGNED_SHORT;
        } else if (value == 6) {
            return BufferDataType::B_INT;
        } else if (value == 7) {
            return BufferDataType::UNSIGNED_INT;
        } else if (value == 8) {
            return BufferDataType::FLOAT;
        } else if (value == 9) {
            assert(0);
        } else {
            assert(0);
        }
    }
    
};

}

#endif /* util_hpp */
