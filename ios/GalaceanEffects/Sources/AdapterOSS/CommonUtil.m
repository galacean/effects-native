//
//  CommonUtil.m
//  MarsNative
//
//  Created by changxing on 2022/10/11.
//  Copyright © 2022 Alipay. All rights reserved.
//

#import "CommonUtil.h"
#import <CommonCrypto/CommonCrypto.h>

@implementation CommonUtil

+ (int) parseDeviceLevelString:(NSString*) str {
    if (!str || str.length == 0) {
        return DEVICE_LEVEL_HIGH;
    }
    if ([[str lowercaseString] isEqualToString:@"high"]) {
        return DEVICE_LEVEL_HIGH;
    }
    if ([[str lowercaseString] isEqualToString:@"medium"]) {
        return DEVICE_LEVEL_MEDIUM;
    }
    if ([[str lowercaseString] isEqualToString:@"low"]) {
        return DEVICE_LEVEL_LOW;
    }
    return DEVICE_LEVEL_HIGH;
}

+ (NSError*)makeError:(NSString*)domain Code:(int)code Msg:(NSString*)msg {
    NSMutableDictionary* details = [NSMutableDictionary dictionary];
    [details setValue:msg forKey:NSLocalizedDescriptionKey];
    return [NSError errorWithDomain:domain code:code userInfo:details];
}

+ (NSString*)getMD5String:(NSString*)str {
    const char* cStr = [str UTF8String];
    unsigned char digest[CC_MD5_DIGEST_LENGTH];
    CC_MD5(cStr, (uint32_t) str.length, digest);
    NSMutableString* result = [NSMutableString stringWithCapacity:CC_MD5_DIGEST_LENGTH * 2];
    for(int i = 0; i < CC_MD5_DIGEST_LENGTH; i++)
    [result appendFormat:@"%02x", digest[i]];
    return result;
}

+ (NSString*)formatTimeStamp:(NSTimeInterval)timeInterval {
    NSDate* date = [NSDate dateWithTimeIntervalSince1970:timeInterval];
    NSDateFormatter* dateFormatter = [[NSDateFormatter alloc] init];
    [dateFormatter setDateFormat:@"yyyy-MM-dd HH:mm:ss"];
    return [dateFormatter stringFromDate: date];
}

@end
