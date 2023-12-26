//
//  earcut.hpp
//  PlayGroundIOS
//
//  Created by changxing on 2022/3/18.
//

#ifndef earcut_hpp
#define earcut_hpp

#include <stdint.h>
#include <vector>

namespace mn {

class Earcut {
public:
    static void Execute(std::vector<uint16_t>& out, const std::vector<float>& data,/* holeIndices,*/ int dim, int index_base);
};

}

#endif /* earcut_hpp */
