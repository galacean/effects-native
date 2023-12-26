#import "MarsStorageUtil.h"

#define KEY_BUSINESS @"MarsNativePlayer"

@implementation MarsStorageUtil

+ (void)setDouble:(double)value forKey:(NSString *)key {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    [defaults setDouble:value forKey:[NSString stringWithFormat:@"%@%@", KEY_BUSINESS, key]];
    [defaults synchronize];
}

+ (double)doubleForKey:(NSString *)key defaultValue:(double)defaultValue {
    NSUserDefaults *defaults = [NSUserDefaults standardUserDefaults];
    NSString *fullKey = [NSString stringWithFormat:@"%@%@", KEY_BUSINESS, key];
    if ([defaults objectForKey:fullKey] == nil) {
        return defaultValue;
    }
    return [defaults doubleForKey:fullKey];
}


+ (NSString*)KEY_LAST_CLEAN_TIMESTAMP {
    return @"MarsNativePurgeTime";
}

@end

#undef KEY_BUSINESS
