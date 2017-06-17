/*
 *  rectangles.c
 *  written by Holmes Futrell
 *  use however you want
 */

#include "SDL.h"
#include <time.h>
#include "Platform.h"

#include "Display.h"

#include "imgui.h"
#include "imgui_impl_sdl_gl3.h"

#include <iostream>

#include <zlib.h>

int main(int argc, char *argv[])
{
    
    // gui
    JustRay::Display display;
    if (!display.Startup()) {
        std::cerr << "ERROR" << std::endl;
        return -1;
    }
    std::cerr << "DISPLAY" << std::endl;
    /* Enter render loop, waiting for user to quit */
    bool show_test_window = false;
    bool show_another_window = false;
    ImVec4 clear_color = ImColor(114, 144, 154);
    bool done = false;
    // Main loop
    while (!done)
    {
        display.UpdateEvent();
        
        // Rendering
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        display.GUIBegin();
        // 1. Show a simple window
        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
        {
            static float f = 0.0f;
            ImGui::Text("Hello, world!");
            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
            ImGui::ColorEdit3("clear color", (float*)&clear_color);
            if (ImGui::Button("Test Window")) show_test_window ^= 1;
            if (ImGui::Button("Another Window")) show_another_window ^= 1;
            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        }
        // 2. Show another simple window, this time using an explicit Begin/End pair
        if (show_another_window)
        {
            ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Another Window", &show_another_window);
            ImGui::Text("Hello");
            ImGui::End();
        }
        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
        if (show_test_window)
        {
            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
            ImGui::ShowTestWindow(&show_test_window);
        }
        display.GUIEnd();
        
    }

    return 0;
}

///*
// *  rectangles.c
// *  written by Holmes Futrell
// *  use however you want
// */
//
//#include "SDL.h"
//#include <time.h>
//#include "Platform.h"
//
//#include "imgui.h"
//#include "imgui_impl_sdl_gl3.h"
//
//#include <iostream>
//
//#include <zlib.h>
//
//int main(int argc, char *argv[])
//{
//    SDL_Window *window;
//
//    /* initialize SDL */
//    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
//        printf("Could not initialize SDL\n");
//        return 1;
//    }
//    
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_ES);
//    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
//    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
//    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
//    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
//    SDL_DisplayMode current;
//    SDL_GetCurrentDisplayMode(0, &current);
//
//    /* create window and renderer */
//    window = SDL_CreateWindow(NULL, 0, 0, current.w, current.h, SDL_WINDOW_OPENGL | SDL_WINDOW_FULLSCREEN);
//    if (!window) {
//        printf("Could not initialize Window\n");
//        return 1;
//    }
//    SDL_GLContext glcontext = SDL_GL_CreateContext(window);
//    glViewport(0,0,current.w,current.h);
//    glClearColor(1.0, 0, 0, 1.0);
//    
//    // gui
//    ImGui_ImplSdlGL3_Init(window);
//    
//    /* Enter render loop, waiting for user to quit */
//    bool show_test_window = false;
//    bool show_another_window = false;
//    ImVec4 clear_color = ImColor(114, 144, 154);
//    bool done = false;
//    // Main loop
//    while (!done)
//    {
//        SDL_Event event;
//        while (SDL_PollEvent(&event))
//        {
//            ImGui_ImplSdlGL3_ProcessEvent(&event);
//            if (event.type == SDL_QUIT)
//                done = true;
//        }
//        ImGui_ImplSdlGL3_NewFrame(window);
//        
//        // 1. Show a simple window
//        // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
//        {
//            static float f = 0.0f;
//            ImGui::Text("Hello, world!");
//            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
//            ImGui::ColorEdit3("clear color", (float*)&clear_color);
//            if (ImGui::Button("Test Window")) show_test_window ^= 1;
//            if (ImGui::Button("Another Window")) show_another_window ^= 1;
//            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//        }
//        
//        // 2. Show another simple window, this time using an explicit Begin/End pair
//        if (show_another_window)
//        {
//            ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
//            ImGui::Begin("Another Window", &show_another_window);
//            ImGui::Text("Hello");
//            ImGui::End();
//        }
//        
//        // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
//        if (show_test_window)
//        {
//            ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
//            ImGui::ShowTestWindow(&show_test_window);
//        }
//        
//        // Rendering
//        glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
//        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
//        glClear(GL_COLOR_BUFFER_BIT);
//        ImGui::Render();
//        SDL_GL_SwapWindow(window);
//    }
//    
//    // Cleanup
//    ImGui_ImplSdlGL3_Shutdown();
//    SDL_GL_DeleteContext(glcontext);
//    SDL_DestroyWindow(window);
//    SDL_Quit();
//
//    return 0;
//}
