#pragma once

#include "VulkanInstance.h"
#include "vulkan/vulkan.h"
#include <vector>
class CVulkanShader
{
public:
	void SetShaderModule(VkShaderModule shaderModule);
	void SetShaderStageFlagBits(VkShaderStageFlagBits shaderStageFlagBits);
	VkShaderModule GetShaderModule();
	VkShaderStageFlagBits GetShaderStageFlagBits();
private:

	VkShaderModule m_ShaderModule;
	VkShaderStageFlagBits m_ShaderStageFlagBits;
};