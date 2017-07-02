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
    std::cerr << display.GetXResolution() << ", " << display.GetYResolution() << std::endl;
    JustRay::ModelViewer* modelViewer = new JustRay::ModelViewer;
    JustRay::RenderEngine renderEngine;
    renderEngine.Startup(display.GetXResolution(), display.GetYResolution());
    JustRay::Application* app = modelViewer;
    app->Startup();
    std::cerr << "START" << std::endl;
    auto input = display.GetInput();
    JustRay::PerformanceTimer performanceTimer;
    
    performanceTimer.Reset();
    // Main loop
    while (display.Running()) {
        performanceTimer.Tick();
        display.UpdateEvent(true);
        // Rendering
        app->Update(performanceTimer.GetDeltaTime(), *input, renderEngine);
        display.RenderGUI();
        display.SwapBackBuffer();
    }
    app->Shutdown();
    return 0;
}
