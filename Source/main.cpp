/*
 *  rectangles.c
 *  written by Holmes Futrell
 *  use however you want
 */

#include "Display.h"
#include "PerformanceTimer.h"
#include "Application.h"
#include "ModelViewer.h"
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
    
    JustRay::ModelViewer* modelViewer = new JustRay::ModelViewer;
    JustRay::RenderEngine renderEngine;
    JustRay::Application* app = modelViewer;
    app->Startup();
    auto input = display.GetInput();
    JustRay::PerformanceTimer performanceTimer;
    performanceTimer.Reset();
    // Main loop
    while (display.Running()) {
        performanceTimer.Tick();
        display.UpdateEvent();
        // Rendering
        glClearColor(1,1,1,1);
        glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_DEPTH_BUFFER_BIT);
        app->Update(performanceTimer.GetDeltaTime(), *input, renderEngine);
        display.RenderGUI();
        display.SwapBackBuffer();
    }
    app->Shutdown();
    return 0;
}
