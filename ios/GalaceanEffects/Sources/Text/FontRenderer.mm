#import "FontRenderer.h"
#import "MarsDataBase.h"
#import "FontFallback.h"
#import "MarsAPLog.h"

@interface FontRenderData : NSObject

@property (nonatomic, assign) NSUInteger length;
@property (nonatomic, assign) unichar* chars;
@property (nonatomic, assign) CGGlyph* glyphs;
@property (nonatomic, assign) CGPoint* positions;
@property (nonatomic, assign) CGSize* advances;

@property (nonatomic, assign) CTFontRef* fonts;
@property (nonatomic, assign) CTFontRef targetFont;

@property (nonatomic, strong) MarsTextContent* content;
@property (nonatomic, assign) float boundWidth;
@property (nonatomic, assign) float lineHeight;

@property (nonatomic, strong) FontRenderData* ellipsis;
@property (nonatomic, assign) int renderOffset;
@property (nonatomic, assign) int renderLength;

@end

@implementation FontRenderData

- (void)setup:(NSString*)str Content:(MarsTextContent*)content Fallback:(MarsFontFallback*)fallback {
    [self setup:str Content:content Fallback:fallback ShouldClip:YES];
}

- (void)setup:(NSString*)str Content:(MarsTextContent*)content Fallback:(MarsFontFallback*)fallback ShouldClip:(BOOL)shouldClip {
    NSUInteger length = str.length;
    
    self.chars = new unichar[length]; // unichar
    self.glyphs = new CGGlyph[length]; // 字体索引数据
    self.positions = new CGPoint[length]; // 位置数据
    self.advances = new CGSize[length]; // 文字大小数据
    self.length = length;
    
    // 字符串转unichar数组
    [str getCharacters:self.chars range:NSMakeRange(0, length)];
    
    // 获取目标字体
    CTFontRef ctFont = [fallback getCTFontAt:content.fontIdx];
    // 判断目标字体是否可以覆盖整个字符串，能够覆盖的字符会自动更新字体索引
    bool coverAll = CTFontGetGlyphsForCharacters(ctFont, self.chars, self.glyphs, length);
    // 为已经拿到索引的字符计算文字大小
    CTFontGetAdvancesForGlyphs(ctFont, kCTFontOrientationDefault, self.glyphs, self.advances, length);
    CGPoint position = CGPointMake(0, 0);
    self.fonts = nullptr;
    
    if (!coverAll) {
        // 没有覆盖所有字符，需要为每个字体设置字体
        self.fonts = new CTFontRef[length];
        memset(self.fonts, 0, length * sizeof(CTFontRef));
        for (int i = 0; i < length; i++) {
            if (self.glyphs[i] == 0) {
                // 目标字体没有覆盖该字体，降级到系统字体
                CTFontRef fallbackFont = [fallback getCTFontForUnichar:self.chars[i] BaseFont:content.fontIdx];
                CTFontGetGlyphsForCharacters(fallbackFont, self.chars + i, self.glyphs + i, 1);
                if (self.glyphs[i] == 0) {
                    // 系统字体也不支持，不画了，跳过
                    self.fonts[i] = ctFont;
                    continue;
                }
                self.fonts[i] = fallbackFont;
                CTFontGetAdvancesForGlyphs(fallbackFont, kCTFontOrientationDefault, self.glyphs + i, self.advances + i, 1);
                // 后面所有没有索引的字体，都检查一遍能否用这个字体覆盖
                for (int j = i + 1; j < length; j++) {
                    if (self.glyphs[j] == 0) {
                        CTFontGetGlyphsForCharacters(fallbackFont, self.chars + j, self.glyphs + j, 1);
                        if (self.glyphs[j] != 0) {
                            self.fonts[j] = fallbackFont;
                            CTFontGetAdvancesForGlyphs(fallbackFont, kCTFontOrientationDefault, self.glyphs + j, self.advances + j, 1);
                        }
                    }
                }
            } else if (self.fonts[i] == NULL) {
                self.fonts[i] = ctFont;
            } // if (self.glyphs[i] == 0) {
        } // for (int i = 0; i < length; i++) {
    } // if (!coverAll) {
    
    // 处理letterSpace
    float letterSpace = [fallback getLetterSpaceAt:content.fontIdx];
    for (NSUInteger i = 0; i < length; i++) {
        self.positions[i] = CGPointMake(position.x, position.y);
        position.x += self.advances[i].width + letterSpace;
    }
    position.x -= letterSpace;
    
    self.content = content;
    self.boundWidth = position.x;
    self.lineHeight = CTFontGetSize(ctFont);
    self.targetFont = ctFont;
    self.renderOffset = 0;
    self.renderLength = (int) length;
    
    if (content.maxWidth > 0 && self.boundWidth > content.maxWidth && shouldClip) {
        float maxWidth = content.maxWidth;
        if (content.overflow == 2) {
            self.ellipsis = [[FontRenderData alloc] init];
            [self.ellipsis setup:content.ellipsisText Content:content Fallback:fallback ShouldClip:NO];
            // maxWidth减去ellipsis宽度
            maxWidth -= self.ellipsis.boundWidth;
            if (content.align == 1) {
                // 居中两端都有ellipsis
                maxWidth -= self.ellipsis.boundWidth;
            }
        }
        
        // 如果ellipsis宽度大于等于maxWidth，直接用ellipisis
        if (maxWidth <= 0) {
            self.renderLength = 0;
        } else {
            if (content.align == 1) {
                [self clipForAlignCenter:maxWidth];
            } else if (content.align == 2) {
                [self clipForAlignRight:maxWidth];
            } else {
                [self clipForAlignLeft:maxWidth];
            }
        }
    }
}

- (void)clipForAlignCenter:(float)maxWidth {
    while (self.boundWidth > maxWidth && self.renderLength >= 2) {
        self.renderLength -= 2;
        ++self.renderOffset;
        [self updateBoundWidth];
    }
    if (self.boundWidth > maxWidth && self.renderLength >= 1) {
        self.renderLength -= 1;
        ++self.renderOffset;
        [self updateBoundWidth];
    }
}

- (void)clipForAlignLeft:(float)maxWidth {
    while (self.boundWidth > maxWidth && self.renderLength >= 1) {
        self.renderLength -= 1;
        [self updateBoundWidth];
    }
}

- (void)clipForAlignRight:(float)maxWidth {
    while (self.boundWidth > maxWidth && self.renderLength >= 1) {
        self.renderLength -= 1;
        ++self.renderOffset;
        [self updateBoundWidth];
    }
}

- (void)updateBoundWidth {
    if (self.renderLength > 0) {
        self.boundWidth = self.positions[self.renderOffset + self.renderLength - 1].x
                - self.positions[self.renderOffset].x
                + self.advances[self.renderOffset + self.renderLength - 1].width;
    } else {
        self.boundWidth = 0;
    }
}

- (void)render:(CGContextRef)context {
    if (self.renderLength == 0) {
        return;
    }
    if (!self.fonts) {
        CTFontDrawGlyphs(self.targetFont, self.glyphs + self.renderOffset, self.positions + self.renderOffset, self.renderLength, context);
    } else {
        for (int i = self.renderOffset; i < self.renderOffset + self.renderLength; i++) {
            CTFontDrawGlyphs(self.fonts[i], self.glyphs + i, self.positions + i, 1, context);
        }
    }
}

- (void)destroy {
    delete[] self.chars;
    delete[] self.glyphs;
    delete[] self.positions;
    delete[] self.advances;
    delete[] self.fonts;
    if (self.ellipsis) {
        [self.ellipsis destroy];
    }
}

@end

@interface FontRenderer ()

@property (nonatomic, strong) MarsFontFallback* fontFallback;

@end

@implementation FontRenderer

- (BOOL)loadFonts:(NSArray<MarsFontInfo*>*)fonts Data:(NSDictionary<NSString*, NSData*>*)fontDatas {
    self.fontFallback = [[MarsFontFallback alloc] init];
    
    for (int i = 0; i < fonts.count; i++) {
        MarsFontInfo* font = [fonts objectAtIndex:i];
        NSData* fontFileData = nil;
        if (font.url.length) {
            fontFileData = [fontDatas objectForKey:font.url];
        }
        if (![self.fontFallback loadFont:font.family FileData:fontFileData Size:font.size Weight:font.weight Italic:(font.style != 0) LetterSpace:font.letterSpace]) {
            return NO;
        }
    }
    
    MNLogInfo(@"FontRenderer.loadFonts success");
    return YES;
}

/// 绘制文字到UIImage上
/// - Parameters:
///   - textInfo: 文字信息
///   - image: 目标图片
- (UIImage*)drawText:(MarsTextInfo*)textInfo onImage:(UIImage*)image {
    if (!image) {
        MNLogInfo(@"FontRenderer.drawText image is nil");
        return nil;
    }
    
    // 创建模版指定大小的透明context
    CGFloat imgWidth = textInfo.width;
    CGFloat imgHeight = textInfo.height;
    UIGraphicsBeginImageContextWithOptions(CGSizeMake(imgWidth, imgHeight), NO, 1);
    // 创建CGContext
    CGContextRef context = UIGraphicsGetCurrentContext();
    CGContextSaveGState(context);
    
    // 绘制背景，直接拉伸
    CGContextSaveGState(context);
    CGContextTranslateCTM(context, 0, imgHeight);
    CGContextScaleCTM(context, 1.0, -1.0); // flip
    CGContextDrawImage(context, CGRectMake(0, 0, imgWidth, imgHeight), image.CGImage);
    CGContextRestoreGState(context);

    // 文字需要上下翻转
    CGContextScaleCTM(context, 1.0, -1.0);
    
    // 开始绘制文字队列
    NSMutableArray<MarsTextContent*>* texts = textInfo.texts;
    for (int i = 0; i < texts.count; i++) {
        MarsTextContent* content = [texts objectAtIndex:i];
        
        FontRenderData* renderData = [[FontRenderData alloc] init];
        [renderData setup:content.text Content:content Fallback:self.fontFallback];
        
        // 画调试用的背景
//        CGContextSetFillColorWithColor(context, [UIColor colorWithRed:1 green:0 blue:0 alpha:0.3f].CGColor);
//        CGContextFillRect(context, CGRectMake(content.x, -content.y - renderData.lineHeight, content.maxWidth, renderData.lineHeight));
        // 设置文字颜色
        CGContextSetFillColorWithColor(context, [FontRenderer convertColor:content.color].CGColor);
        CGContextSaveGState(context);
        // 处理位移
        CGContextTranslateCTM(context, content.x, -renderData.lineHeight - content.y);
        
        // 处理align
        if (content.align == 1) {
            CGContextTranslateCTM(context, (content.maxWidth - renderData.boundWidth) / 2, 0);
        } else if (content.align == 2) {
            CGContextTranslateCTM(context, content.maxWidth - renderData.boundWidth, 0);
        }
        
        if (renderData.ellipsis) {
            if (content.align == 1 || content.align == 2) {
                CGContextTranslateCTM(context, -renderData.ellipsis.boundWidth, 0);
                [renderData.ellipsis render:context];
                CGContextTranslateCTM(context, renderData.ellipsis.boundWidth, 0);
            }
        }
        // 处理裁切后的水平位移
        CGContextTranslateCTM(context,  -renderData.positions[renderData.renderOffset].x, 0);
        [renderData render:context];
        if (renderData.ellipsis) {
            if (content.align != 2) {
                CGContextTranslateCTM(context, renderData.positions[renderData.renderOffset].x + renderData.boundWidth, 0);
                [renderData.ellipsis render:context];
            }
        }
        
        CGContextRestoreGState(context);
        
        [renderData destroy];
    }
    
    CGContextRestoreGState(context);
    
    UIImage* newImg = UIGraphicsGetImageFromCurrentImageContext();
    UIGraphicsEndImageContext();
    
    return newImg;
}

- (void)clean {
    [self.fontFallback clear];
}

#pragma mark - private

+ (UIColor*)convertColor:(uint32_t)rgba {
    float r, g, b, a;
    a = ((float)(rgba & 0xff)) / 255.0f;
    rgba >>= 8;
    b = ((float)(rgba & 0xff)) / 255.0f;
    rgba >>= 8;
    g = ((float)(rgba & 0xff)) / 255.0f;
    rgba >>= 8;
    r = ((float)(rgba & 0xff)) / 255.0f;
    return [UIColor colorWithRed:r green:g blue:b alpha:a];
}

@end
