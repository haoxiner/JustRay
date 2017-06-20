#pragma once
#include "Platform.h"
namespace JustRay
{
class Input {
friend class Display;
public:
//    void TouchBegin(int x, int y);
//    void TouchMove(int x, int y);
//    void TouchEnd(int x, int y);
//    void Pinch
    bool Move() const;
    float GetX() const;
    float GetY() const;
private:
    void Update(const SDL_Event& event);
private:
    bool touching_ = false;
    float x_;
    float y_;
    int fingerCount_ = 0;
    bool moving_ = false;
};
}
