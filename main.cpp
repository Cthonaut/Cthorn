#include "include/application.h"

#include <iostream>

const std::vector<const char *> validationLayers = {"VK_LAYER_KHRONOS_validation"};

int main()
{
    Cthorn::Application app;

    app.enableValidationLayers = true;
    app.validationLayers = validationLayers;

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
