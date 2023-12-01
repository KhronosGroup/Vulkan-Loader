#include "vk_loader_platform.h"

#import <Foundation/Foundation.h>

// https://developer.apple.com/documentation/foundation/nsprocessinfo?language=objc
char *loader_platform_executable_path(char *buffer, size_t size) {
    NSString *executable_path = [[NSProcessInfo processInfo] arguments][0];

    const char * result = [executable_path UTF8String];

    const size_t result_length = strlen(result);
    if (result_length > size) {
        return NULL;
    }
    return memcpy(buffer, result, result_length);
}
