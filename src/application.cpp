#include "../include/application.h"

namespace Cthorn
{

void Application::initSdl2()
{
    CHECK(!SDL_Init(SDL_INIT_VIDEO));
    SDL_Vulkan_LoadLibrary(nullptr);
    window = SDL_CreateWindow("Vulkan", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 360,
                              SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN);
}

void Application::initVulkan()
{
    std::sort(device.VL.begin(), device.VL.end());
    std::sort(device.deviceExt.begin(), device.deviceExt.end());
    device.initInstance(window);
    if (device.useVL)
    {
        device.setupDebugMessenger();
    }
    CHECK(SDL_Vulkan_CreateSurface(window, device.instance, &device.surface))
    device.selectGPU();
    device.initLogDevice();
    device.initSwapChain(window);
    device.initImageViews();

    graphics.device = device;
    graphics.shaders.push_back(graphics.initShader(vertShaderLocation.data(), VK_SHADER_STAGE_VERTEX_BIT));
    graphics.shaders.push_back(graphics.initShader(fragShaderLocation.data(), VK_SHADER_STAGE_FRAGMENT_BIT));
    graphics.initRenderPass();
    graphics.initPipeline();
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
    graphics.DestroyShaders();
    graphics.cleanup();

    if (device.useVL)
        device.vkDestroyDebugUtilsMessengerEXT(device.instance, device.debugMessenger, nullptr);

    device.cleanupSwapChain();
    device.cleanup();
    SDL_DestroyWindow(window);
    SDL_Vulkan_UnloadLibrary();
    SDL_Quit();
}

} // namespace Cthorn
