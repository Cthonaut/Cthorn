#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "device.h"
#include "graphics.h"

namespace Cthorn
{
class Application
{
  private:
    SDL_Window *window;

    void initSdl2();
    void initVulkan();
    void loop();
    void cleanup();

  public:
    Device device;
    Graphics graphics;

    std::string vertShaderLocation;
    std::string fragShaderLocation;

    void run();
};
} // namespace Cthorn
