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

#include <algorithm>
#include <iostream>
#include <optional>
#include <set>
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
    std::vector<const char *> getRequiredExtensions(SDL_Window *window, bool enableValidationLayers);

    bool extensionSupport(VkPhysicalDevice device);
    bool getQueueFamilies(VkPhysicalDevice device, std::vector<uint32_t> *queueFamilies);

  public:
    VkDebugUtilsMessengerEXT debugMessenger;
    VkInstance instance;
    VkSurfaceKHR surface;
    VkPhysicalDevice phyDevice;
    VkDevice logDevice;
    VkSwapchainKHR swapChain;
    VkQueue graphicsQueue;
    VkQueue presentQueue;

    bool useVL = false;
    std::vector<const char *> VL;
    std::vector<const char *> deviceExt;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;

    PFN_vkCreateDebugUtilsMessengerEXT vkCreateDebugUtilsMessengerEXT;
    PFN_vkDestroyDebugUtilsMessengerEXT vkDestroyDebugUtilsMessengerEXT;

    void initInstance(SDL_Window *window);
    void setupDebugMessenger();
    void selectGPU();
    void initLogDevice();
    void initSwapChain(SDL_Window *window);
    void cleanup();
};

static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessage(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                   VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                   const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                   void *pUserData);
} // namespace Cthorn
