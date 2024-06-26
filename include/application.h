#pragma once

#include <stdexcept>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "device.h"

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

    void run();
};
} // namespace Cthorn
