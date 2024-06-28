#include "../include/graphics.h"
#include <cstdint>

namespace Cthorn
{

Shader Graphics::initShader(const char *shaderLocation, VkShaderStageFlagBits shaderStage)
{
    Shader shader;

    std::ifstream file(shaderLocation, std::ios::ate | std::ios::binary);
    CHECK(file.is_open());
    size_t fileSize = (size_t)file.tellg();
    std::vector<char> shaderCode(fileSize);
    file.seekg(0);
    file.read(shaderCode.data(), fileSize);
    file.close();

    VkShaderModuleCreateInfo shaderModuleInfo{};
    shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleInfo.codeSize = fileSize;
    shaderModuleInfo.pCode = reinterpret_cast<const uint32_t *>(shaderCode.data());
    VK_CHECK(vkCreateShaderModule(device.logDevice, &shaderModuleInfo, nullptr, &shader.shaderModule))

    shader.shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader.shaderStageInfo.stage = shaderStage;
    shader.shaderStageInfo.module = shader.shaderModule;
    shader.shaderStageInfo.pName = "main";

    return shader;
}

void Graphics::DestroyShaders()
{
    for (int32_t i = 0; i < shaders.size(); i++)
    {
        vkDestroyShaderModule(device.logDevice, shaders[i].shaderModule, nullptr);
    }
}

} // namespace Cthorn
