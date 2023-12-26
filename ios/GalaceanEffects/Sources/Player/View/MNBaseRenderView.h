//
//  MNBaseRenderView.h
//  MarsNative
//
//  Created by changxing on 2023/11/1.
//  Copyright © 2023 Alipay. All rights reserved.
//

#ifndef MNBaseRenderView_h
#define MNBaseRenderView_h

#import <UIKit/UIKit.h>

@protocol MNBaseRenderView <NSObject>

- (CALayer *)getViewLayer;

@end

#endif /* MNBaseRenderView_h */
