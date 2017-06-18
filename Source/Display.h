#pragma once
#include "Input.h"
#include "Platform.h"
namespace JustRay
{
class Display {
public:
    bool Startup();
    void UpdateEvent();
    void SwapBackBuffer();
    void RenderGUI();
    void Shutdown();
    bool Running();
    const Input* GetInput() const;
    int GetXResolution() const;
    int GetYResolution() const;
private:
    bool running_ = false;
    SDL_Window* window_ = nullptr;
    Input input_;
    int xResolution_;
    int yResolution_;
};
}
