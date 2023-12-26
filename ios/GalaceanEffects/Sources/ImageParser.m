//
//  ImageParser.m
//
//  Created by Zongming Liu on 2022/3/1.
//

#import "ImageParser.h"

@implementation ImageParser

+(void)parseUIImageToBitmap:(vImage_Buffer *)dest_buffer image:(UIImage *)image {
    if (!dest_buffer) {
        return;
    }
    CGImageRef imageRef = image.CGImage;
    [ImageParser CGImageDecodeToBitmapBufferWith32BitFormat:imageRef buffer:dest_buffer];
}

+(BOOL)CGImageDecodeToBitmapBufferWith32BitFormat:(CGImageRef)srcImage buffer:(vImage_Buffer *)dest
{
    if (!srcImage || !dest) return NO;
    size_t width = CGImageGetWidth(srcImage);
    size_t height = CGImageGetHeight(srcImage);
    if (width == 0 || height == 0) {
        NSLog(@"parse image to bitmap fail, width: %d, height: %d", (int)width, (int)height);
        return NO;
    }
    
    /*
     Try convert with vImageConvert_AnyToAny() (avaliable since iOS 7.0).
     If fail, try decode with CGContextDrawImage().
     CGBitmapContext use a premultiplied alpha format, unpremultiply may lose precision.
     */
    vImage_CGImageFormat destFormat = {0};
    destFormat.bitsPerComponent = 8;
    destFormat.bitsPerPixel = 32;
    destFormat.colorSpace = CGColorSpaceCreateDeviceRGB();
    destFormat.bitmapInfo = kCGBitmapByteOrderDefault | kCGImageAlphaLast;
    dest->data = NULL;
    
    CGContextRef context = NULL;
    
    do {
        CGBitmapInfo contextBitmapInfo = kCGBitmapByteOrderDefault | kCGImageAlphaPremultipliedLast;
        context = CGBitmapContextCreate(NULL, width, height, 8, width * 4, CGColorSpaceCreateDeviceRGB(), contextBitmapInfo);
        if (!context) {
            NSLog(@"parse image to bitmap fail, context is nil");
            break;
        }
        // flip Y;
        CGContextTranslateCTM(context, 0, height);
        CGContextScaleCTM(context, 1.0f, -1.0f);
        
        CGContextDrawImage(context, CGRectMake(0, 0, width, height), srcImage); // decode and convert

        size_t bytesPerRow = CGBitmapContextGetBytesPerRow(context);
        size_t length = height * bytesPerRow;
        void *data = CGBitmapContextGetData(context);
        if (length == 0 || !data) {
            NSLog(@"parse image to bitmap fail, data length:%ld", length);
            break;
        }
        
        dest->data = malloc(length);
        dest->width = width;
        dest->height = height;
        dest->rowBytes = bytesPerRow;
        if (!dest->data) {
            NSLog(@"parse image to bitmap fail, data is nil");
            break;
        }
        memcpy(dest->data, data, length);
        
        CFRelease(context);
        return YES;
    } while(0);

    if (context) {
        CFRelease(context);
        context = nil;
    }
    if (dest->data) free(dest->data);
    dest->data = NULL;
    return NO;
}

@end
