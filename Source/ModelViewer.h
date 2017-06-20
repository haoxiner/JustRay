//
//  ModelViewer.h
//  JustRay
//
//  Created by xin on 2017/6/18.
//
//

#pragma once
#include "Application.h"
#include "ModelGroup.h"
#include "MathUtil.h"
#include <memory>
#include <vector>
#include <utility>
namespace JustRay
{
class ModelViewer : public Application
{
public:
    void Startup() override;
    void Shutdown() override;
    void Update(float deltaTime, const Input& input, RenderEngine& renderEngine) override;
private:
    Matrix4x4 modelToWorld_;
    std::unique_ptr<ModelGroup> testModel_;
};
}

