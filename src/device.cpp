#include "../include/device.h"

namespace Cthorn
{

void Device::initInstance(SDL_Window *window)
{
    if (useVL)
    {
        uint32_t layerCount;
        vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
        std::vector<VkLayerProperties> availableLayers(layerCount);
        vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
        std::vector<std::string> availableLayersNames(layerCount);
        for (uint32_t i = 0; i < layerCount; i++)
        {
            availableLayersNames.push_back(availableLayers[i].layerName);
        }
        std::sort(availableLayersNames.begin(), availableLayersNames.end());

        if (!std::includes(availableLayersNames.begin(), availableLayersNames.end(), VL.begin(), VL.end()))
            throw std::runtime_error("validation layers not available");
    }

    VkApplicationInfo formalInfo{};
    formalInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    formalInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
    formalInfo.pApplicationName = "Demo";
    formalInfo.pEngineName = "Cthorn";

    uint32_t sdl2ExtensionCount;
    CHECK(SDL_Vulkan_GetInstanceExtensions(window, &sdl2ExtensionCount, nullptr));
    std::vector<const char *> extensions(sdl2ExtensionCount);
    CHECK(SDL_Vulkan_GetInstanceExtensions(window, &sdl2ExtensionCount, extensions.data()));
    if (useVL)
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

    VkInstanceCreateInfo instInfo{};
    instInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instInfo.pApplicationInfo = &formalInfo;
    instInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instInfo.ppEnabledExtensionNames = extensions.data();
    instInfo.enabledLayerCount = 0;
    if (useVL)
    {
        instInfo.enabledLayerCount = static_cast<uint32_t>(VL.size());
        instInfo.ppEnabledLayerNames = VL.data();
    }
    VK_CHECK(vkCreateInstance(&instInfo, nullptr, &instance));
}

void Device::selectGPU()
{
    phyDevice = VK_NULL_HANDLE;

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0)
        throw std::runtime_error("found no GPUs");
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    for (uint32_t i = 0; i < deviceCount; i++)
    {
        if (!getQueueFamilies(devices[i], nullptr))
            continue;

        uint32_t extensionCount;
        vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, nullptr);
        std::vector<VkExtensionProperties> availableExt(extensionCount);
        vkEnumerateDeviceExtensionProperties(devices[i], nullptr, &extensionCount, availableExt.data());
        std::vector<std::string> availableExtNames(extensionCount);
        for (uint32_t i = 0; i < extensionCount; i++)
        {
            availableExtNames[i] = availableExt[i].extensionName;
        }
        std::sort(availableExtNames.begin(), availableExtNames.end());
        if (!std::includes(availableExtNames.begin(), availableExtNames.end(), deviceExt.begin(), deviceExt.end()))
            continue;

        uint32_t availableFormatsCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(devices[i], surface, &availableFormatsCount, nullptr);
        if (availableFormatsCount == 0)
            continue;

        uint32_t availablePresentModesCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(devices[i], surface, &availablePresentModesCount, nullptr);
        if (availablePresentModesCount == 0)
            continue;

        phyDevice = devices[i];
        break;
    }

    if (phyDevice == VK_NULL_HANDLE)
    {
        throw std::runtime_error("sutable GPU not found");
    }
}

bool Device::getQueueFamilies(VkPhysicalDevice device, std::vector<uint32_t> *queueFamilies)
{
    std::optional<uint32_t> graphicsFamilyopt;
    std::optional<uint32_t> presentFamilyopt;
    bool assigned = false;

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamiliesList(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamiliesList.data());

    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        if (!presentFamilyopt.has_value())
        {
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            if (presentSupport)
                presentFamilyopt = i;
        }

        if (queueFamiliesList[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            graphicsFamilyopt = i;

        if (graphicsFamilyopt.has_value() && presentFamilyopt.has_value())
        {
            assigned = true;
            break;
        }
    }

    if (queueFamilies != nullptr)
    {
        queueFamilies->push_back(graphicsFamilyopt.value());
        if (graphicsFamilyopt.value() != presentFamilyopt.value())
        {
            queueFamilies->push_back(presentFamilyopt.value());
        }
    }
    return assigned;
}

void Device::initLogDevice()
{
    std::vector<uint32_t> queueFamilies;
    getQueueFamilies(phyDevice, &queueFamilies);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueCount = 1;
    float priority = 1.0f;
    queueCreateInfo.pQueuePriorities = &priority;
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos(queueFamilies.size());
    for (uint32_t i = 0; i < queueFamilies.size(); i++)
    {
        queueCreateInfos[i] = queueCreateInfo;
        queueCreateInfos[i].queueFamilyIndex = queueFamilies[i];
    }

    VkDeviceCreateInfo logdInfo{};
    logdInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    logdInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    logdInfo.pQueueCreateInfos = queueCreateInfos.data();
    VkPhysicalDeviceFeatures deviceFeatures{};
    logdInfo.pEnabledFeatures = &deviceFeatures;
    logdInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExt.size());
    logdInfo.ppEnabledExtensionNames = deviceExt.data();
    logdInfo.enabledLayerCount = 0;
    if (useVL)
    {
        logdInfo.enabledLayerCount = static_cast<uint32_t>(VL.size());
        logdInfo.ppEnabledLayerNames = VL.data();
    }

    VK_CHECK(vkCreateDevice(phyDevice, &logdInfo, nullptr, &logDevice));

    // retrieve queue handle
    vkGetDeviceQueue(logDevice, queueFamilies[0], 0, &graphicsQueue);
    if (queueCreateInfos.size() > 1)
        vkGetDeviceQueue(logDevice, queueFamilies[1], 0, &presentQueue);
}

void Device::initSwapChain(SDL_Window *window)
{
    VkSurfaceCapabilitiesKHR capabilities;
    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(phyDevice, surface, &capabilities);

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }

    VkExtent2D extent;
    if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        extent = capabilities.currentExtent;
    }
    else
    {
        int width, height;
        SDL_GL_GetDrawableSize(window, &width, &height);

        VkExtent2D actualExtent = {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};

        actualExtent.width =
            std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
        actualExtent.height =
            std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

        extent = actualExtent;
    }

    VkSurfaceFormatKHR supFormat;
    uint32_t availableFormatsCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(phyDevice, surface, &availableFormatsCount, nullptr);
    std::vector<VkSurfaceFormatKHR> availableFormats(availableFormatsCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(phyDevice, surface, &availableFormatsCount, availableFormats.data());
    supFormat = availableFormats[0];
    for (int i = 0; i < availableFormatsCount; i++)
    {
        if (availableFormats[i].format == VK_FORMAT_B8G8R8A8_SRGB &&
            availableFormats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            supFormat = availableFormats[i];
        }
    }

    std::vector<uint32_t> queueFamilies;
    getQueueFamilies(phyDevice, &queueFamilies);

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
    uint32_t availablePresentModesCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(phyDevice, surface, &availablePresentModesCount, nullptr);
    std::vector<VkPresentModeKHR> availablePresentModes(availablePresentModesCount);
    vkGetPhysicalDeviceSurfacePresentModesKHR(phyDevice, surface, &availablePresentModesCount,
                                              availablePresentModes.data());
    for (int i = 0; i < availablePresentModesCount; i++)
    {
        if (availablePresentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR)
        {
            presentMode = availablePresentModes[i];
        }
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = supFormat.format;
    createInfo.imageColorSpace = supFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (queueFamilies.size() > 1)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilies.data();
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }
    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    VK_CHECK(vkCreateSwapchainKHR(logDevice, &createInfo, nullptr, &swapChain));

    vkGetSwapchainImagesKHR(logDevice, swapChain, &imageCount, nullptr);
    swapChainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(logDevice, swapChain, &imageCount, swapChainImages.data());

    swapChainImageFormat = supFormat.format;
    swapChainExtent = extent;
}

void Device::cleanup()
{
    vkDestroySwapchainKHR(logDevice, swapChain, nullptr);
    vkDestroyDevice(logDevice, nullptr);
    vkDestroySurfaceKHR(instance, surface, nullptr);
    vkDestroyInstance(instance, nullptr);
}

void Device::setupDebugMessenger()
{
    VkDebugUtilsMessengerCreateInfoEXT dbugInfo;
    dbugInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    dbugInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
                               VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    dbugInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
                           VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    dbugInfo.pfnUserCallback = debugMessage;
    dbugInfo.flags = 0;
    dbugInfo.pNext = NULL;

    vkCreateDebugUtilsMessengerEXT =
        (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (vkCreateDebugUtilsMessengerEXT == nullptr ||
        vkCreateDebugUtilsMessengerEXT(instance, &dbugInfo, nullptr, &debugMessenger))
    {
        throw std::runtime_error("failed to set up debug messenger");
    }

    vkDestroyDebugUtilsMessengerEXT =
        (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (vkDestroyDebugUtilsMessengerEXT == nullptr)
    {
        throw std::runtime_error("failed to set up debug destroyer");
    }
}

static VKAPI_ATTR VkBool32 VKAPI_CALL debugMessage(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                   VkDebugUtilsMessageTypeFlagsEXT messageType,
                                                   const VkDebugUtilsMessengerCallbackDataEXT *pCallbackData,
                                                   void *pUserData)
{
    std::string prefix;
    if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
    {
        return VK_FALSE;
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
    {
        prefix = "VL_INFO: ";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
    {
        prefix = "VL_WARNING: ";
    }
    else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
    {
        prefix = "VL_ERROR: ";
    }

    std::cerr << prefix << pCallbackData->pMessage << std::endl;

    return VK_FALSE;
}

} // namespace Cthorn
