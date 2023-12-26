#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface MarsStorageUtil : NSObject

+ (void)setDouble:(double)value forKey:(NSString*)key;

+ (double)doubleForKey:(NSString*)key defaultValue:(double)defaultValue;

+ (NSString*)KEY_LAST_CLEAN_TIMESTAMP;

@end

NS_ASSUME_NONNULL_END
