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
#include "Entity.h"
#include "MathUtil.h"
#include <memory>
#include <vector>
#include <utility>
#include <map>
#include <string>
namespace JustRay
{
class ModelViewer : public Application
{
public:
    void Startup() override;
    void Shutdown() override;
    void Update(float deltaTime, const Input& input, RenderEngine& renderEngine) override;
private:
    // control entity rotation
    bool rotating_ = false;
    Float3 v0;
    Float3 cameraPosition_;
    Matrix4x4 initRotation_;
    
    // render
    Matrix4x4 modelToWorld_;
    std::shared_ptr<Entity> entity_;
    std::unique_ptr<ModelGroup> testModel_;
    std::map<const std::string, std::shared_ptr<Material>> materialMap_;
    
    std::unique_ptr<Material> customMaterial_;
    
    // control
    bool useCustomMaterial_ = false;
    Float3 customBaseColor_ = Float3(1.0f);
    float customRoughness_ = 0.5f;
    float customMetallic_ = 0.0f;
};
}

