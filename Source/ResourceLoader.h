//
//  ResourceLoader.h
//  JustRay
//
//  Created by xin on 2017/6/18.
//
//

#pragma once
#include <string>
namespace JustRay
{
class ResourceLoader
{
public:
    static std::string LoadFileAsString(const std::string& filepath);
private:
};
}

