#include "include/application.h"

#include <iostream>

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};
const std::vector<const char *> deviceExtensions = {VK_KHR_SWAPCHAIN_EXTENSION_NAME}; // graphics card can render images

int main()
{
    Cthorn::Application app;

    app.enableValidationLayers = true;
    app.validationLayers = validationLayers;
    app.deviceExtensions = deviceExtensions;

    try
    {
        app.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
