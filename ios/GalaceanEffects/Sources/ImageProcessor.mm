#import "ImageProcessor.h"
#import <UIKit/UIKit.h>
#import "MarsDataBase.h"
#import "FontRenderer.h"
#import "MarsAPLog.h"
#import "ImageParser.h"
#import "MarsNativeResourceLoader.h"
#include "image_loader.h"

@interface MarsImageDataInfo ()

@property (nonatomic, strong) UIImage* image;

@end

@implementation MarsImageDataInfo
@end

@interface MarsImageProcessor ()

@property (nonatomic, strong) NSMutableDictionary<NSString*, MarsImageDataInfo*>* imageDataInfos;

@property (nonatomic, strong) NSArray<MarsNativeImageResource*>* resList;

@property (nonatomic, strong) NSArray<MarsTextInfo*>* texts;

@property (nonatomic, strong) NSArray<MarsFontInfo*>* fonts;

@property (nonatomic, strong, nullable) NSDictionary<NSString*, NSData*>* fontDatas;

@end

@implementation MarsImageProcessor

- (instancetype)initWithResList:(NSArray* _Nullable)resList {
    if (self = [super init]) {
        self.imageDataInfos = [[NSMutableDictionary alloc] init];
        self.resList = resList;
        for (int i = 0; i < resList.count; i++) {
            MarsNativeImageResource* res = [resList objectAtIndex:i];
            MarsImageDataInfo* info = [[MarsImageDataInfo alloc] init];
            if (res.isVideo) {
                info.isVideo = YES;
                info.videoRes = res.videoRes;
            } else if (mn::ImageLoader::IsKtxImage((const uint8_t*) res.data.bytes, res.data.length)) {
                info.isKtx = YES;
                info.data = res.data;
            } else {
                info.isKtx = NO;
                if (res.bitmap) {
                    info.image = res.bitmap;
                } else {
                    info.image = [UIImage imageWithData:res.data];
                }
                info.width = info.image.size.width;
                info.height = info.image.size.height;
            }
            [self.imageDataInfos setValue:info forKey:res.key];
        }
    }
    return self;
}

- (void)setTexts:(NSArray*)texts Fonts:(NSArray*)fonts {
    self.texts = texts;
    self.fonts = fonts;
}

- (void)setFontDataMap:(NSDictionary<NSString*, NSData*>*)fontDataMap {
    self.fontDatas = fontDataMap;
}

- (NSDictionary<NSString*, MarsImageDataInfo*>*)processWithVariables:(NSDictionary*)variables {
    if (self.texts) {
        FontRenderer* fontRenderer = [[FontRenderer alloc] init];
        if (![fontRenderer loadFonts:self.fonts Data:self.fontDatas]) {
            return nil;
        }
        for (int i = 0; i < self.texts.count; i++) {
            MarsTextInfo* textInfo = [self.texts objectAtIndex:i];
            NSString* imgUrl = [self.resList objectAtIndex:textInfo.imgIdx].key;
            if (!imgUrl) {
                MNLogInfo(@"fail to find imgUrl for %d", textInfo.imgIdx);
                return nil;
            }
            MarsImageDataInfo* info = [self.imageDataInfos objectForKey:imgUrl];
            if (!info) {
                MNLogInfo(@"fail to find image for %@", imgUrl);
                return nil;
            }
            if (info.isKtx) {
                MNLogInfo(@"can't draw text on ktx %@", imgUrl);
                return nil;
            }
            // 填充文字动态数据
            for (int j = 0; j < textInfo.texts.count; j++) {
                MarsTextContent* content = [textInfo.texts objectAtIndex:j];
                if (content.name.length && [variables objectForKey:content.name]) {
                    content.text = [variables objectForKey:content.name];
                }
            }
            info.image = [fontRenderer drawText:textInfo onImage:info.image];
            if (!info.image) {
                MNLogInfo(@"fail to draw text on ktx %@", imgUrl);
                return nil;
            }
            info.width = info.image.size.width;
            info.height = info.image.size.height;
        }
        [fontRenderer clean];
    }
    for (NSString* key in self.imageDataInfos) {
        MarsImageDataInfo* info = [self.imageDataInfos objectForKey:key];
        if (info.isKtx) {
            continue;
        }
        vImage_Buffer dest_buffer = {0};
        [ImageParser parseUIImageToBitmap:&dest_buffer image:info.image];
        size_t width = dest_buffer.width;
        size_t height = dest_buffer.height;
        info.image = nil;
        info.data = [NSData dataWithBytesNoCopy:dest_buffer.data length:(width * height * 4) freeWhenDone:YES];
    }
    
    return self.imageDataInfos;
}

- (void)releaseData {
    self.imageDataInfos = nil;
    self.texts = nil;
    self.fonts = nil;
    self.fontDatas = nil;
}

@end
