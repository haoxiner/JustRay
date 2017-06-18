//
//  Application.h
//  JustRay
//
//  Created by xin on 2017/6/18.
//
//

#pragma once
#include "RenderEngine.h"
#include "Input.h"
#include "imgui.h"
namespace JustRay
{
class Application
{
public:
    virtual void Startup() = 0;
    virtual void Shutdown() = 0;
    virtual void Update(float deltaTime, const Input& input, RenderEngine& renderEngine) = 0;
private:
};
}

