//
//  ModelViewer.cpp
//  JustRay
//
//  Created by xin on 2017/6/18.
//
//

#include "ModelViewer.h"
namespace JustRay
{
void ModelViewer::Startup()
{
    testModel_.reset(new ModelGroup("mitsuba"));
}
void ModelViewer::Shutdown()
{
}
void ModelViewer::Update(float deltaTime, const Input& input, RenderEngine& renderEngine)
{
    ImGui::Text("Hello, world!");
    renderEngine.Render(*testModel_);
}
};
