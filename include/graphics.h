#pragma once

#include <fstream>
#include <vulkan/vulkan_core.h>

#include "device.h"

namespace Cthorn
{

struct Shader
{
    VkPipelineShaderStageCreateInfo shaderStageInfo{};
    VkShaderModule shaderModule{};
};

struct Pipeline
{
    VkPipelineLayout layout;
    VkPipeline graphicsPipeline;
};

class Graphics
{
  public:
    Device device;
    std::vector<Shader> shaders;
    Pipeline pipeline;
    VkRenderPass renderPass;

    Shader initShader(const char *shaderLocation, VkShaderStageFlagBits shaderStage);
    void DestroyShaders();
    void initRenderPass();
    void initPipeline();
    void cleanup();
};

} // namespace Cthorn
