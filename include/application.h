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
    Device device;

    void initSdl2();
    void initVulkan();
    void loop();
    void cleanup();

  public:
    bool enableValidationLayers;
    std::vector<const char *> validationLayers;
    std::vector<const char *> deviceExtensions;

    void run();
};
} // namespace Cthorn
