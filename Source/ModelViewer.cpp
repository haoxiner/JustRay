//
//  ModelViewer.cpp
//  JustRay
//
//  Created by xin on 2017/6/18.
//
//

#include "ModelViewer.h"
#include <iostream>
namespace JustRay
{
void ModelViewer::Startup()
{
    testModel_.reset(new ModelGroup("mitsuba", materialMap_));
    entity_.reset(new Entity);
    entity_->rotation_ = Quaternion(0,0,0,1);
    testModel_->entities.emplace_back(entity_);
    modelToWorld_ = Matrix4x4(1.0f);
    cameraPosition_ = Float3(0,-3,1);
}
void ModelViewer::Shutdown()
{
}
void ModelViewer::Update(float deltaTime, const Input& input, RenderEngine& renderEngine)
{
    static bool flag = true;
    if (flag) {
        flag = false;
        renderEngine.SetEnvironment("uffizi");
    }
    if (input.Move()) {
        if (!rotating_) {
            rotating_ = true;
            v0.x = input.GetX();
            v0.y = input.GetY();
            initRotation_ = glm::mat4_cast(entity_->rotation_);
        } else {
            float xRot = -PI * (input.GetY() - v0.y);
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
        cameraPosition_.y += input.Pinch() * 10;
        cameraPosition_.y = std::fminf(-1, cameraPosition_.y);
    }
    renderEngine.SetCamera(cameraPosition_, Float3(0,cameraPosition_.y + 1,cameraPosition_.z), Float3(0,0,1));
//    glClearColor(0.0, 0.0, 0.0, 0.0);
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//    renderEngine.Render(*testModel_);
//    ImGui::Text("Hello, world!");
    renderEngine.Clear();
    renderEngine.RenderToGBuffer(*testModel_);
    renderEngine.SubmitToScreen();
}
};
