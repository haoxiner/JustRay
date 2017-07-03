//
//  SkyBox.h
//  JustRay
//
//  Created by xin on 2017/7/3.
//
//

#pragma once
#include "Platform.h"
#include <memory>
namespace JustRay
{
class SkyBox
{
public:
    SkyBox();
    void Render();
private:
    GLuint vertexArrayID_;
    GLuint vertexBufferID_;
    GLuint indexBufferID_;
};
}

