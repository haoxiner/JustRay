//
//  ModelViewer.cpp
//  JustRay
//
//  Created by xin on 2017/6/18.
//
//

#include "ModelViewer.h"
#include "Json.h"
#include <iostream>
namespace JustRay
{
void ModelViewer::Startup()
{
    testModel_.reset(new ModelGroup(modelList[currentModelIndex_], materialMap_));
    entity_.reset(new Entity);
    entity_->rotation_ = Quaternion(0,0,0,1);
    testModel_->entities.emplace_back(entity_);
    modelToWorld_ = Matrix4x4(1.0f);
    cameraPosition_ = Float3(0,3,0);
    
    customMaterial_.reset(new Material(1.0, 1.0, 1.0, 1.0, 0.5, 1.0));
}
void ModelViewer::Shutdown()
{
}
void ModelViewer::Update(float deltaTime, const Input& input, RenderEngine& renderEngine)
{
    static bool firstTime = true;
    if (firstTime) {
        firstTime = false;
        renderEngine.SetEnvironment("uffizi");
    }
    if (input.Move()) {
        if (!rotating_) {
            rotating_ = true;
            v0.x = input.GetX();
            v0.y = input.GetY();
            initRotation_ = glm::mat4_cast(entity_->rotation_);
        } else {
            float xRot = PI * (input.GetY() - v0.y);
            float zRot = -PI * (input.GetX() - v0.x);
            Matrix4x4 rot;
            rot = glm::rotate(rot, xRot, glm::vec3(1,0,0));
            rot = glm::rotate(rot, zRot, glm::vec3(0,0,1));
            entity_->rotation_ = glm::quat_cast(initRotation_ * rot);
        }
    } else if (rotating_) {
        rotating_ = false;
    }
    if (input.Pinch()) {
        cameraPosition_.y -= input.Pinch() * 10;
        cameraPosition_.y = std::fmaxf(1, cameraPosition_.y);
    }
    
    bool showWindow = true;
    ImGui::SetNextWindowPos(ImVec2(5.0f, 5.0f));
    ImGui::SetNextWindowSize(ImVec2(180.0f, 60.0f));
    ImGui::Begin("Panel", &showWindow, ImGuiWindowFlags_NoMove);
    ImGui::Spacing();
    ImGui::Checkbox("Show Controller", &showController_);
    ImGui::End();
    
    if (showController_) {
        ImGui::SetNextWindowPos(ImVec2(10.0f, renderEngine.GetYResolution() - 250.0f));
        ImGui::SetNextWindowSize(ImVec2(renderEngine.GetXResolution() - 20.0f, 240.0f));
        ImGui::Begin("Controller", &showWindow, ImGuiWindowFlags_NoMove);
        
        controllerType_ = ImGui::Button("  View  ") ? 0 : controllerType_;
        ImGui::SameLine();
        controllerType_ = ImGui::Button("  Material  ") ? 1 : controllerType_;
        ImGui::SameLine();
        controllerType_ = ImGui::Button("  Model  ") ? 2 : controllerType_;
        ImGui::SameLine();
        controllerType_ = ImGui::Button("  Environment  ") ? 3 : controllerType_;
        
        ImGui::Spacing();
        
        if (controllerType_ == 0) {
            ImGui::SliderFloat("AO", &aoScale_, 0.0f, 10.0f);
            ImGui::SliderFloat("Exposure", &exposure_, 0.0f, 10.0f);
            renderEngine.SetEffect(aoScale_, exposure_);
        }else if (controllerType_ == 1) {
            ImGui::Checkbox("Custom Material", &useCustomMaterial_);
            ImGui::Spacing();
            if (useCustomMaterial_) {
                ImGui::Spacing();
                ImGui::ColorEdit3("Base Color", &customBaseColor_[0]);
                ImGui::Spacing();
                ImGui::SliderFloat("Roughness", &customRoughness_, 0.0f, 1.0f);
                ImGui::Spacing();
                ImGui::SliderFloat("Metallic", &customMetallic_, 0.0f, 1.0f);
                ImGui::Spacing();
                customMaterial_->Set(false, customBaseColor_.x, customBaseColor_.y, customBaseColor_.z, customMetallic_, customRoughness_, 1.0);
            } else {
                auto old = currentMaterialIndex_;
                if (ImGui::ListBox("Material List", &currentMaterialIndex_, materialList, 4)) {
                    if (old != currentMaterialIndex_) {
                        if (currentMaterialIndex_) {
                            uniformMaterial_.reset(new Material(materialList[currentMaterialIndex_], 4.0));
                        }
                    }
                }
                if (currentMaterialIndex_) {
                    float uvScale = uniformMaterial_->GetUVScale();
                    ImGui::SliderFloat("UV Scale", &uvScale, 0.0f, 10.0f);
                    uniformMaterial_->SetTexCoordScale(uvScale);
                }
            }
        } else if (controllerType_ == 2) {
            auto old = currentModelIndex_;
            if (ImGui::ListBox("Model List", &currentModelIndex_, modelList, 3)) {
                if (old != currentModelIndex_) {
                    materialMap_.clear();
                    testModel_.reset(new ModelGroup(modelList[currentModelIndex_], materialMap_));
                    testModel_->entities.emplace_back(entity_);
                }
            }
        } else if (controllerType_ == 3) {
            auto old = currentEnvironmentIndex_;
            if (ImGui::ListBox("Environment List", &currentEnvironmentIndex_, environmentList, 1)) {
                if (old != currentEnvironmentIndex_) {
                    renderEngine.SetEnvironment(environmentList[currentEnvironmentIndex_]);
                }
            }
        }
        ImGui::End();
    }
    renderEngine.SetCamera(cameraPosition_, Float3(0,cameraPosition_.y - 1,cameraPosition_.z), Float3(0,0,1));
    renderEngine.Prepare();
    if (useCustomMaterial_) {
        renderEngine.Submit(*testModel_, *customMaterial_);
    } else if (currentMaterialIndex_) {
        renderEngine.Submit(*testModel_, *uniformMaterial_);
    } else {
        renderEngine.Submit(*testModel_);
    }
    renderEngine.SubmitToScreen();
}
};
