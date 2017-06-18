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
}
void ModelViewer::Shutdown()
{
}
void ModelViewer::Update(float deltaTime, const Input& input, RenderEngine& renderEngine)
{
    ImGui::Text("Hello, world!");
    
    for (const auto& pair : models_) {
        auto indexOffset = pair.first;
        auto numOfIndices = pair.second;
        
    }
}
};
