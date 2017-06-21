#include "Input.h"
#include "Platform.h"
#include <iostream>
namespace JustRay
{
void Input::Update(const SDL_Event& event)
{
    moving_ = false;
    pinch_ = 0.0f;
    if (event.type == SDL_FINGERDOWN) {
    } else if (event.type == SDL_FINGERUP) {
        if (fingerCount_ > 0) {
            fingerCount_--;
        }
    } else if (event.type == SDL_FINGERMOTION) {
        if (fingerCount_ <= 1) {
            moving_ = true;
            x_ = event.tfinger.x;
            y_ = event.tfinger.y;
        }
    } else if (event.type == SDL_MULTIGESTURE) {
        fingerCount_ = event.mgesture.numFingers;
        pinch_ = event.mgesture.dDist;
    }
}
bool Input::Move() const
{
    return moving_;
}
float Input::Pinch() const
{
    return pinch_;
}
float Input::GetX() const
{
    return x_;
}
float Input::GetY() const
{
    return y_;
}
}
