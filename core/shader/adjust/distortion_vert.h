//
//  distortion_vert.h
//  MarsNative
//
//  Created by Zongming Liu on 2022/8/24.
//  Copyright © 2022 Alipay. All rights reserved.
//

#ifndef distortion_vert_h
#define distortion_vert_h

namespace mn {

const std::string DISTORTION_VERT = R"(

    uniform vec4 uMovementValue;
    uniform vec4 uStrengthValue;
    uniform vec4 uPeriodValue;
    varying vec4 vWaveParams;

    void filterMain(float lifetime){
        const float pi2 = 3.14159265;
        vWaveParams = vec4(
        getValueFromTime(lifetime, uPeriodValue) * pi2,
        getValueFromTime(lifetime, uMovementValue) * pi2,
        getValueFromTime(lifetime, uStrengthValue),
        0.);
    }
)";

}

#endif /* distortion_vert_h */
