#pragma once

#include <fstream>
#include <string>
#include <vector>

#include <vulkan/vulkan.h>

#include "device.h"

namespace Cthorn
{

struct Shader
{
    VkPipelineShaderStageCreateInfo shaderStageInfo;
    VkShaderModule shaderModule;
};

class Graphics
{
  private:
  public:
    Device device;
    std::vector<Shader> shaders;

    Shader initShader(const char *shaderLocation, VkShaderStageFlagBits shaderStage);
    void DestroyShaders();
};

} // namespace Cthorn
