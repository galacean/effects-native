//
//  MarsPositionPathModifierExtension.m
//  MarsNative
//
//  Created by changxing on 2023/9/12.
//  Copyright © 2023 Alipay. All rights reserved.
//

#import "MarsPositionPathModifierExtension.h"
#import "CommonUtil.h"
#include "extensions/mars_ext_position_path_modifier.hpp"

@interface MarsPositionPathModifierExtension ()

@property (nonatomic, strong) NSMutableArray<NSString*>* itemNames;

@property (nonatomic, assign) mn::MarsExtPositionPathModifier* modifier;

@end

@implementation MarsPositionPathModifierExtension

- (instancetype)initWithParams:(NSArray<NSString*>*)params {
    if (self = [super init]) {
        self.itemNames = [params copy];
        self.modifier = new mn::MarsExtPositionPathModifier();
    }
    return self;
}

// sceneData创建成功
- (BOOL)onSceneDataCreated:(nonnull void*)sceneData Error:(NSError**)error {
    if (!self.modifier->SetSceneData((mn::SceneDataDecoder*) sceneData)) {
        if (error) {
            *error = [CommonUtil makeError:@"MarsPositionPathModifierExtension"
                                      Code:0
                                       Msg:@"SetSceneData fail"];
        }
        return NO;
    }
    [self onSceneDataParamsPixelRatio:self.modifier->GetPixelRatio()
                               Aspect:self.modifier->GetAspect()
                            AnimWidth:self.modifier->GetAnimWidth()
                           AnimHeight:self.modifier->GetAnimHeight()];
    for (int i = 0; i < self.itemNames.count; i++) {
        NSString* name = [self.itemNames objectAtIndex:i];
        float* data = nullptr;
        float* data2 = nullptr;
        int length = 0;
        int length2 = 0;
        if (!self.modifier->GetPositionPathByName(name.UTF8String, &data, &length, &data2, &length2)) {
            if (error) {
                *error = [CommonUtil makeError:@"MarsPositionPathModifierExtension"
                                          Code:0
                                           Msg:@"GetPositionPathByName fail"];
            }
            return NO;
        }
        MarsPositionPathModifyResult* result = [self modifyItemPathData:name
                                                                   Type:data2 ? @"bezier" : @"vec3"
                                                                  Value:data
                                                                 Length:length
                                                                 Value2:data2
                                                                Length2:length2];
        if (!result) {
            if (error) {
                *error = [CommonUtil makeError:@"MarsPositionPathModifierExtension"
                                          Code:0
                                           Msg:@"modifyItemPathData fail"];
            }
            return NO;
        }
        self.modifier->ModifyPositionPath(name.UTF8String, result.data, result.length, result.data2, result.length2);
    }
    return YES;
}

- (void*)getCustomPlugin:(NSString**)name {
    return nullptr;
}

- (void)onDestroy {
    // 销毁
    if (self.modifier) {
        delete self.modifier;
        self.modifier = nullptr;
    }
}

- (void)onSceneDataParamsPixelRatio:(float)pixelRatio
                             Aspect:(float)aspect
                          AnimWidth:(float)animWidth
                         AnimHeight:(float)animHeight {
}

- (MarsPositionPathModifyResult*)modifyItemPathData:(NSString*)itemName
                                               Type:(NSString*)type
                                              Value:(float*)value
                                             Length:(int)length
                                             Value2:(float*)value2
                                            Length2:(int)length2 {
    return nil;
}

@end

@implementation MarsPositionPathModifyResult

@end
