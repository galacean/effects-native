#import <Foundation/Foundation.h>
#import <CoreText/CoreText.h>

NS_ASSUME_NONNULL_BEGIN

@interface MarsFontFallback : NSObject

- (instancetype)init;

- (BOOL)loadFont:(NSString*)fontName
        FileData:(NSData*)fileData
            Size:(float)size
          Weight:(int)weight
          Italic:(BOOL)italic
     LetterSpace:(float)letterSpace;

+ (CGAffineTransform*)getMatrix:(BOOL)italic;

- (void)clear;

- (CTFontRef)getCTFontAt:(int)idx;

- (float)getLetterSpaceAt:(int)idx;

- (CTFontRef)getCTFontForUnichar:(unichar)uch BaseFont:(int)baseIdx;

@end

NS_ASSUME_NONNULL_END
