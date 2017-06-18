#include "Display.h"
#include "imgui.h"
#include "imgui_impl_sdl_gl3.h"
#include "Platform.h"
#include <iostream>

namespace JustRay
{
bool Display::Startup()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "FAIL";
        return false;
    }
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    
    SDL_DisplayMode displayMode;
    SDL_GetCurrentDisplayMode(0, &displayMode);
    xResolution_ = displayMode.w;
    yResolution_ = displayMode.h;
    
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    //SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 8);

    window_ = SDL_CreateWindow(nullptr, 0, 0, xResolution_, yResolution_, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
    if (window_ == nullptr) {
        
        return false;
    }
    SDL_GLContext glContext = SDL_GL_CreateContext(window_);
    if (!glContext) {
        return false;
    }
//    if (SDL_GL_SetSwapInterval(1) < 0) {
//        return false;
//    }
//
//    SDL_GL_SetSwapInterval(1);
    //glEnable(GL_MULTISAMPLE);
    running_ = true;
    
    ImGui_ImplSdlGL3_Init(window_);
    return true;
}

void Display::UpdateEvent()
{
    SDL_Event event;
    while (SDL_PollEvent(&event) != 0) {
        ImGui_ImplSdlGL3_ProcessEvent(&event);
        if (event.type == SDL_QUIT) {
            running_ = false;
        }
        input_.UpdateKeyboard(SDL_GetKeyboardState(nullptr));
    }
    ImGui_ImplSdlGL3_NewFrame(window_);
}

void Display::SwapBackBuffer()
{
    SDL_GL_SwapWindow(window_);
}

void Display::RenderGUI()
{
    glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
    ImGui::Render();
}

void Display::Shutdown() {
    running_ = false;
    // Cleanup
    ImGui_ImplSdlGL3_Shutdown();
    if (window_) {
        SDL_DestroyWindow(window_);
        window_ = nullptr;
    }
    SDL_Quit();
}

bool Display::Running() {
    return running_;
}

const Input* Display::GetInput() const {
    return &input_;
}
}
