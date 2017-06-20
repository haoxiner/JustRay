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
    testModel_.reset(new ModelGroup("mitsuba"));
    modelToWorld_ = Matrix4x4(1.0f);
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
        std::cerr << input.GetX() << ", " << input.GetY() << std::endl;
    }
//    ImGui::Text("Hello, world!");
    renderEngine.Render(*testModel_);
}
};
