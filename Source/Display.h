#pragma once
#include "Input.h"
#include "Platform.h"
namespace JustRay
{
class Display {
public:
    bool Startup();
    void UpdateEvent(bool waitForEvent = false);
    void SwapBackBuffer();
    void RenderGUI();
    void Shutdown();
    bool Running();
    const Input* GetInput() const;
    int GetXResolution() const { return xResolution_; }
    int GetYResolution() const { return yResolution_; }
private:
    bool running_ = false;
    SDL_Window* window_ = nullptr;
    Input input_;
    int xResolution_;
    int yResolution_;
};
}
