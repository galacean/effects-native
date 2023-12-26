//
//  FontFallback.m
//  MarsNative
//
//  Created by changxing on 2023/4/17.
//  Copyright © 2023 Alipay. All rights reserved.
//

#import "FontFallback.h"
#import <UIKit/UIKit.h>
#import "MarsAPLog.h"

static CGAffineTransform s_italic_matrix;
static CGAffineTransform s_identity_matrix;

/// 持有字体句柄
@interface MarsFontHolder : NSObject

@property (nonatomic, assign) CTFontRef ctFont; // 渲染用的
@property (nonatomic, assign) CGFontRef cgFont; // 保证注册到系统的字体不被释放

@property (nonatomic, assign) float size; // 字体大小
@property (nonatomic, assign) int weight;
@property (nonatomic, assign) bool italic;
@property (nonatomic, assign) float letterSpace;

- (instancetype)initWithSize:(float)size Weight:(int)weight Italic:(bool)italic LetterSpace:(float)letterSpace;
- (void)clear;

@end

@implementation MarsFontHolder

- (instancetype)initWithSize:(float)size Weight:(int)weight Italic:(bool)italic LetterSpace:(float)letterSpace {
    if (self = [super init]) {
        self.size = size;
        self.weight = weight;
        self.italic = italic;
        self.letterSpace = letterSpace;
        MNLogInfo(@"MarsFallbackFont.init %p %f %d %d %f", self, size, weight, italic, letterSpace);
    }
    return self;
}

- (void)clear {
    MNLogInfo(@"MarsFallbackFont.clear %p", self);
    if (self.ctFont) {
        CFRelease(self.ctFont);
        self.ctFont = nil;
    }
    if (self.cgFont != NULL) {
        // 自定义字体，取消注册
        CTFontManagerUnregisterGraphicsFont(self.cgFont, NULL);
        CGFontRelease(self.cgFont);
    }
}

@end

@interface MarsFontFallback ()

/// 用过的fallback字体名
@property (nonatomic, strong) NSMutableArray<NSString*>* fallbackList;

/// 所有fontRef对象
@property (nonatomic, strong) NSMutableArray<MarsFontHolder*>* fontCache;

/// 记录原始字体数量，fallback字体不在原始字体里找
@property (nonatomic, assign) NSUInteger originFontCount;

@end

@implementation MarsFontFallback

- (instancetype)init {
    if (self = [super init]) {
        self.fallbackList = [[NSMutableArray alloc] init];
        self.fontCache = [[NSMutableArray alloc] init];
        self.originFontCount = 0;
    }
    return self;
}

/// 加载字体
- (BOOL)loadFont:(NSString*)fontName
        FileData:(NSData*)fileData
            Size:(float)size
          Weight:(int)weight
          Italic:(BOOL)italic
     LetterSpace:(float)letterSpace {
    MNLogInfo(@"MarsFontFallback.loadFont %@ %@", fontName, fileData);
    
    MarsFontHolder* marsFont = [[MarsFontHolder alloc] initWithSize:size Weight:weight Italic:italic LetterSpace:letterSpace];
    UIFontDescriptor* desc;
    if (!fileData) {
        // 没有文件，加载系统字体
        desc = [UIFontDescriptor fontDescriptorWithName:fontName size:size];
    } else {
        // 加载自定义字体
        CGDataProviderRef provider = CGDataProviderCreateWithCFData((__bridge CFDataRef) fileData);
        CGFontRef fontRef = CGFontCreateWithDataProvider(provider);
        NSString* realFontName = CFBridgingRelease(CGFontCopyPostScriptName(fontRef));
        CGDataProviderRelease(provider);
        if (CTFontManagerRegisterGraphicsFont(fontRef, NULL)) {
            marsFont.cgFont = fontRef;
        }
        desc = [UIFontDescriptor fontDescriptorWithName:realFontName size:size];
        if (!marsFont.cgFont) {
            CGFontRelease(fontRef);
            MNLogInfo(@"create cgFont fail");
            return NO;
        }
    }
    UIFont* uiFont = [UIFont fontWithDescriptor:desc size:size];
    
    NSMutableDictionary* attr = [[NSMutableDictionary alloc] initWithObjectsAndKeys:
                                 uiFont.familyName, (__bridge NSString*)kCTFontFamilyNameAttribute,
                                 nil];
    if (weight > 450) {
        NSDictionary* traits = @{(id)kCTFontWeightTrait:[NSNumber numberWithDouble:UIFontWeightBold]};
        [attr setObject:traits forKey:(__bridge NSString*)kCTFontTraitsAttribute];
    } else if (weight < 350) {
        NSDictionary* traits = @{(id)kCTFontWeightTrait:[NSNumber numberWithDouble:UIFontWeightLight]};
        [attr setObject:traits forKey:(__bridge NSString*)kCTFontTraitsAttribute];
    }
    
    CTFontDescriptorRef descriptor = CTFontDescriptorCreateWithAttributes((__bridge CFDictionaryRef)attr);
    marsFont.ctFont = CTFontCreateWithFontDescriptor(descriptor, size, [MarsFontFallback getMatrix:italic]);
    CFRelease(descriptor);
    
    [self.fontCache addObject:marsFont];
    self.originFontCount = self.fontCache.count;
    return YES;
}

/// 获取斜体的渲染矩阵，统一通过矩阵实现斜体
+ (CGAffineTransform*)getMatrix:(BOOL)italic {
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        s_italic_matrix = CGAffineTransformMake(1, 0, tanf(10 * (CGFloat)M_PI / 180), 1, 0, 0);
        s_identity_matrix = CGAffineTransformIdentity;
    });
    return italic ? &s_italic_matrix : &s_identity_matrix;
}

- (void)clear {
    MNLogInfo(@"MarsFontFallback.clear");
    for (int i = 0; i < self.fontCache.count; i++) {
        [self.fontCache[i] clear];
    }
    [self.fontCache removeAllObjects];
    [self.fallbackList removeAllObjects];
}

#pragma mark - private

/// 检查当前字体是否支持指定字符
- (bool)fontCover:(CTFontRef)fontRef Unichar:(unichar)uch {
    CGGlyph glyphs;
    CTFontGetGlyphsForCharacters(fontRef, &uch, &glyphs, 1);
    return (glyphs > 0);
}

- (CTFontRef)getCTFontAt:(int)idx {
    return [self.fontCache objectAtIndex:idx].ctFont;
}

- (float)getLetterSpaceAt:(int)idx {
    return [self.fontCache objectAtIndex:idx].letterSpace;
}

- (CTFontRef)getCTFontForUnichar:(unichar)uch BaseFont:(int)baseIdx {
    // 检查已有的fontRef是否支持
    MarsFontHolder* baseFont = [self.fontCache objectAtIndex:baseIdx];
    for (NSUInteger i = self.originFontCount; i < self.fontCache.count; i++) {
        MarsFontHolder* font = [self.fontCache objectAtIndex:i];
        if (font.size == baseFont.size && font.weight == baseFont.weight &&
                font.italic == baseFont.italic && font.letterSpace == baseFont.letterSpace &&
                [self fontCover:font.ctFont Unichar:uch]) {
            return font.ctFont;
        }
    }
    // 检查fallback过的字体名是否支持
    for (int i = 0; i < _fallbackList.count; i++) {
        NSString* name = _fallbackList[i];
        CTFontRef fontRef = CTFontCreateCopyWithFamily(baseFont.ctFont, 0, NULL, (__bridge CFStringRef)name);
        if ([self fontCover:fontRef Unichar:uch]) {
            MarsFontHolder* font = [[MarsFontHolder alloc] initWithSize:baseFont.size Weight:baseFont.weight Italic:baseFont.italic LetterSpace:baseFont.letterSpace];
            font.ctFont = fontRef;
            [_fontCache addObject:font];
            return fontRef;
        }
        CFRelease(fontRef);
    }
    MNLogInfo(@"MarsFontFallback loadSystemFont %u", uch);
    // 从系统字体创建fallback字体
    NSArray* languages = [[NSUserDefaults standardUserDefaults] stringArrayForKey:@"AppleLanguages"];
    CFArrayRef languagesCf = (__bridge CFArrayRef)languages;

    CFArrayRef arr = CTFontCopyDefaultCascadeListForLanguages(baseFont.ctFont, languagesCf);
    for (int i = 0; i < CFArrayGetCount(arr); i++) {
        CTFontDescriptorRef fontDescRef = (CTFontDescriptorRef)CFArrayGetValueAtIndex(arr, i);
        CTFontRef fontRef = CTFontCreateWithFontDescriptor(fontDescRef, baseFont.size, [MarsFontFallback getMatrix:baseFont.italic]);
        if ([self fontCover:fontRef Unichar:uch]) {
            MarsFontHolder* font = [[MarsFontHolder alloc] initWithSize:baseFont.size Weight:baseFont.weight Italic:baseFont.italic LetterSpace:baseFont.letterSpace];
            font.ctFont = fontRef;
            [_fontCache addObject:font];
            CFStringRef name = CTFontCopyFamilyName(fontRef);
            [_fallbackList addObject:(__bridge NSString*)name];
            CFRelease(name);
            CFRelease(arr);
            return fontRef;
        }
        CFRelease(fontRef);
    }
    CFRelease(arr);
    return NULL;
}

@end
