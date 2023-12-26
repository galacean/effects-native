//
//  MarsCustomMorphExtension.m
//  MarsNative
//
//  Created by changxing on 2023/9/14.
//  Copyright © 2023 Alipay. All rights reserved.
//

#import "MarsCustomMorphExtension.h"
#include "extensions/morph/custom_morph.hpp"

@interface MarsCustomMorphExtension ()

@property (nonatomic, strong) NSArray<NSString*>* params;

@end

@implementation MarsCustomMorphExtension

- (instancetype)initWithParams:(NSArray<NSString*>*) params {
    if (self = [super init]) {
        self.params = params;
    }
    return self;
}

- (void*)getCustomPlugin:(NSString**)name {
    if (name) {
        *name = @"morph";
    }
    std::vector<std::string> params;
    for (int i = 0; i < self.params.count; i++) {
        params.push_back([self.params objectAtIndex:i].UTF8String);
    }
    return new mn::CustomMorphPluginBuilder(params, [self](const std::string& name, float time, float** points, int* points_length, uint16_t** indices, int* indices_length) {
        if (!points) {
            return;
        }
        NSArray* result = [self generateMorphData:[NSString stringWithUTF8String:name.c_str()] Time:time];
        if (!result) {
            return;
        }
        NSArray* pArr = [result objectAtIndex:0];
        NSArray* iArr = [result objectAtIndex:1];
        *points_length = (int) pArr.count;
        *points = new float[*points_length];
        *indices_length = (int) iArr.count;
        *indices = new uint16_t[*indices_length];
        for (int i = 0; i < *points_length; i++) {
            (*points)[i] = [[pArr objectAtIndex:i] floatValue];
        }
        for (int i = 0; i < *indices_length; i++) {
            (*indices)[i] = [[iArr objectAtIndex:i] floatValue];
        }
    });
}

- (BOOL)onSceneDataCreated:(void *)sceneData Error:(NSError *__autoreleasing *)error {
    float duration = 0;
    do {
        if (!sceneData) {
            break;
        }
        const auto& compositions = ((mn::SceneDataDecoder*) sceneData)->compositions_;
        if (compositions.empty()) {
            break;
        }
        duration = compositions[0]->duration;
    } while (false);
    [self onSceneDataParamsDuration:duration];
    return YES;
}

- (void)onDestroy {
}

- (void)onSceneDataParamsDuration:(float)duration {
}

- (NSArray*)generateMorphData:(NSString*)name Time:(float)time {
    return nil;
}

@end
