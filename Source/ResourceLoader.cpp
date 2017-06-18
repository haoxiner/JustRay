//
//  ResourceLoader.cpp
//  JustRay
//
//  Created by xin on 2017/6/18.
//
//

#include "ResourceLoader.h"
#include "AppleSystemTools.h"
#include <fstream>
namespace JustRay
{
std::string ResourceLoader::LoadFileAsString(const std::string& filepath)
{
    std::ifstream file(GetFilePath(filepath));
    std::istreambuf_iterator<char> fileBegin(file), fileEnd;
    return std::string(fileBegin, fileEnd);
}
};
