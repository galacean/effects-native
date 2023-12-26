#import "MarsDataJsonBin.h"
#import <UIKit/UIKit.h>
#import "MarsAPLog.h"

@interface MarsDataJsonBin()

@property (nonatomic, strong) NSString* binPath;

@end

@implementation MarsDataJsonBin

- (instancetype)initWithJsonPath:(NSString*)jsonPath BinPath:(NSString*)binPath {
    if (self = [super init]) {
        if (![self loadJson:jsonPath]) {
            MNLogInfo(@"MarsDataJsonBin:loadJson fail");
            return self;
        }
        self.binPath = binPath;
        self.valid = true;
    }
    return self;
}

- (MarsDataType)getType {
    return MarsDataType::JSON_BIN;
}

- (NSString*)getBinPath {
    return self.binPath;
}

#pragma mark - private method

- (BOOL)loadJson:(NSString*)jsonPath {
    @try {
        NSData* data = [NSData dataWithContentsOfFile:jsonPath];
        if (!data || data.length == 0) {
            // mars.json读取失败
            MNLogInfo(@"MarsDataJsonBin:empty bytes");
            return NO;
        }
        NSError* err = nil;
        NSDictionary* scene = [NSJSONSerialization JSONObjectWithData:data options:NSJSONReadingAllowFragments error:&err];
        if (err) {
            // mars.json的内容转成JSONObject失败
            MNLogInfo(@"MarsDataJsonBin:parse json fail,%@", err);
            return NO;
        }
        
        self.version = [scene objectForKey:@"version"];

        NSDictionary* meta = [scene objectForKey:@"meta"];
        NSArray* previewSize = [meta objectForKey:@"previewSize"];
        self.previewSize = CGSizeMake([[previewSize objectAtIndex:0] floatValue], [[previewSize objectAtIndex:1] floatValue]);

        self.duration = [[scene objectForKey:@"duration"] floatValue];

        NSMutableArray<MarsImageInfo*>* imageList = [[NSMutableArray alloc] init];
        NSArray* images = [scene objectForKey:@"images"];
        MNLogInfo(@"MarsDataJsonBin:images count %lu", images.count);
        
        // 只有iOS 15及以上系统支持webp
        BOOL deviceSupportWebp = ([UIDevice currentDevice].systemVersion.doubleValue >= 15.0);
        
        for (int i = 0; i < images.count; i++) {
            NSDictionary* image = [images objectAtIndex:i];
            MarsImageInfo* info = [[MarsImageInfo alloc] init];
            info.url = [image objectForKey:@"url"];
            if (deviceSupportWebp && [image objectForKey:@"webp"]) {
                NSString* webp = [image objectForKey:@"webp"];
                if (webp.length) {
                    info.url = webp;
                }
            }
            info.isVideo = NO;
            if ([image objectForKey:@"isVideo"]) {
                info.isVideo = [[image objectForKey:@"isVideo"] boolValue];
            }
            info.isTransparentVideo = NO;
            if (info.isVideo) {
                if ([image objectForKey:@"isTransparent"]) {
                    info.isTransparentVideo = [[image objectForKey:@"isTransparent"] boolValue];
                }
            }
            info.templateIdx = -1;
            if ([image objectForKey:@"templateIdx"]) {
                info.templateIdx = [[image objectForKey:@"templateIdx"] intValue];
            }
            // 默认开启压缩纹理
            if ([image objectForKey:@"compressed"]) {
                NSDictionary* compressed = [image objectForKey:@"compressed"];
                if ([compressed objectForKey:@"astc"]) {
                    info.astc = [compressed objectForKey:@"astc"];
                    if (!info.astc.length) {
                        info.astc = nil;
                    }
                }
            }
            [imageList addObject:info];
        }
        
        self.images = imageList;
        
        NSDictionary* marsTexts = [scene objectForKey:@"marsTexts"];
        if (marsTexts) {
            NSArray* textsData = [marsTexts objectForKey:@"texts"];
            NSArray* fontsData = [marsTexts objectForKey:@"fonts"];
            NSMutableArray* texts = [[NSMutableArray alloc] init];
            NSMutableArray* fonts = [[NSMutableArray alloc] init];
            
            for (int i = 0; i < textsData.count; i++) {
                NSDictionary* data = [textsData objectAtIndex:i];
                MarsTextInfo* text = [[MarsTextInfo alloc] init];
                text.imgIdx = [[data objectForKey:@"imgIdx"] intValue];
                text.x = [[data objectForKey:@"x"] floatValue];
                text.y = [[data objectForKey:@"y"] floatValue];
                text.width = [[data objectForKey:@"width"] floatValue];
                text.height = [[data objectForKey:@"height"] floatValue];
                
                NSArray* contentsData = [data objectForKey:@"texts"];
                NSMutableArray* contents = [[NSMutableArray alloc] init];
                for (int j = 0; j < contentsData.count; j++) {
                    NSArray* contentData = [contentsData objectAtIndex:j];
                    MarsTextContent* content = [[MarsTextContent alloc] init];
                    content.x = [[contentData objectAtIndex:0] floatValue];
                    content.y = [[contentData objectAtIndex:1] floatValue];
                    content.name = [contentData objectAtIndex:2];
                    content.text = [contentData objectAtIndex:3];
                    content.maxWidth = [[contentData objectAtIndex:4] floatValue];
                    content.fontIdx = [[contentData objectAtIndex:5] intValue];
                    content.align = [[contentData objectAtIndex:6] intValue];
                    content.color = [[contentData objectAtIndex:7] unsignedIntValue];
                    content.overflow = [[contentData objectAtIndex:8] intValue];
                    content.ellipsisText = [contentData objectAtIndex:9];
                    [contents addObject:content];
                }
                
                text.texts = contents;
                [texts addObject:text];
            }
            for (int i = 0; i < fontsData.count; i++) {
                NSDictionary* data = [fontsData objectAtIndex:i];
                MarsFontInfo* font = [[MarsFontInfo alloc] init];
                font.url = [data objectForKey:@"url"];
                font.weight = [[data objectForKey:@"weight"] intValue];
                font.size = [[data objectForKey:@"size"] floatValue];
                font.letterSpace = [[data objectForKey:@"letterSpace"] floatValue];
                font.family = [data objectForKey:@"family"];
                font.style = [[data objectForKey:@"style"] intValue];
                [fonts addObject:font];
            }
        
            if (!fonts.count) {
                return NO;
            }
            self.fonts = fonts;
            self.texts = texts;
        }
        return YES;
    } @catch (NSException *exception) {
        MNLogInfo(@"MarsDataJsonBin:loadJson..e:%@", exception);
        return NO;
    }
}

- (float)getAspect {
    float w = self.previewSize.width;
    float h = self.previewSize.height;
    if (w == 0 || h == 0) {
        return 1;
    }
    return w / h;
}

@end
