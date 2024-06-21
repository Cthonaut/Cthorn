#pragma once

#define VK_CHECK(result)                                                                                               \
    if (result != VK_SUCCESS)                                                                                          \
    {                                                                                                                  \
        throw std::runtime_error("Vulkan error at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));         \
    }

#define CHECK(result)                                                                                                  \
    if (!result)                                                                                                       \
    {                                                                                                                  \
        throw std::runtime_error("Error at " + std::string(__FILE__) + ":" + std::to_string(__LINE__));                \
    }

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_vulkan.h>
#include <vulkan/vulkan.h>

namespace Cthorn
{
class Device
{
  private:
    bool checkValidationLayerSupport(const std::vector<const char *> validationLayers);
    std::vector<const char *> getRequiredExtensions(SDL_Window *window, bool enableValidationLayers);

  public:
    VkDebugUtilsMessengerEXT debugMessenger;
    VkInstance instance;

    void createInstance(SDL_Window *window, const std::vector<const char *> validationLayers,
                        bool enableValidationLayers);
    void setupDebugMessenger();
    void cleanupDebugMessenger();
    void cleanup();
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                    VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                    const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                    void *pUserData);
} // namespace Cthorn
