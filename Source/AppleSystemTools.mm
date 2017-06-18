//
//  AppleSystemTools.mm
//  JustRay
//
//  Created by xin on 2017/6/18.
//
//

#import <Foundation/Foundation.h>
#include "AppleSystemTools.h"

namespace JustRay
{
std::string GetFilePath(const std::string& fileID)
{
    NSString* path = [NSString stringWithUTF8String:fileID.c_str()];
    NSString* realPath = [[NSBundle mainBundle] pathForResource:path ofType:nil];
    return std::string([realPath UTF8String]);
}
}
