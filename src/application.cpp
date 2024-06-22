#include "../include/application.h"

namespace Cthorn
{

void Application::initSdl2()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Vulkan_LoadLibrary(nullptr);
    window = SDL_CreateWindow("Vulkan", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 360,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
}

void Application::initVulkan()
{
    device.createInstance(window, validationLayers, enableValidationLayers);
    if (enableValidationLayers)
    {
        device.setupDebugMessenger();
    }
    if (SDL_Vulkan_CreateSurface(window, device.instance, &device.surface) != SDL_TRUE)
    {
        throw std::runtime_error("failed to create surface");
    }
    device.selectGPU(deviceExtensions);
    SDL_Log("Initializing sdl errors: %s", SDL_GetError());
}

void Application::loop()
{
    bool running = true;
    while (running)
    {
        SDL_Event windowEvent;
        while (SDL_PollEvent(&windowEvent))
            if (windowEvent.type == SDL_QUIT)
            {
                running = false;
                break;
            }
    }
}

void Application::run()
{
    initSdl2();
    initVulkan();
    loop();
    cleanup();
}

void Application::cleanup()
{
    if (enableValidationLayers)
    {
        device.cleanupDebugMessenger();
    }
    device.cleanup();
    SDL_DestroyWindow(window);
    SDL_Vulkan_UnloadLibrary();
    SDL_Quit();
    SDL_Log("Closing sdl errors: %s", SDL_GetError());
}

} // namespace Cthorn
