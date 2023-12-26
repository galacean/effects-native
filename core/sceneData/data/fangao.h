//
//  fangao.h
//  PlayGroundIOS
//
//  Created by changxing on 2022/8/10.
//

#ifndef fangao_h
#define fangao_h

// 梵高特殊数据
namespace mn {

struct FangaoReplace {
    std::string name;
    float x;
    float y;
    float w;
    float h;
    
    float offset_l = 0;
    float offset_r = 0;
    float offset_t = 0;
    float offset_b = 0;
    
    FangaoReplace(std::string name, float x, float y, float w, float h) {
        this->name = name;
        this->x = x;
        this->y = y;
        this->w = w;
        this->h = h;
    }
};

struct FangaoFollow {
    std::string name;
    std::string auto_parent;
    std::string auto_type;
    
    FangaoFollow(std::string name, std::string auto_parent, std::string auto_type) {
        this->name = name;
        this->auto_parent = auto_parent;
        this->auto_type = auto_type;
    }
};

struct FangaoData {
    std::vector<FangaoReplace> replace;
    std::vector<FangaoFollow> follow;
};

}

#endif /* fangao_h */
