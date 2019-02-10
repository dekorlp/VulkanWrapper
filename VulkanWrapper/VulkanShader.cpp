#include "VulkanShader.h"


void CVulkanShader::SetShaderModule(VkShaderModule shaderModule)
{
	this->m_ShaderModule = shaderModule;
}

void CVulkanShader::SetShaderStageFlagBits(VkShaderStageFlagBits shaderStageFlagBits)
{
	this->m_ShaderStageFlagBits = shaderStageFlagBits;
}

VkShaderModule CVulkanShader::GetShaderModule()
{
	return this->m_ShaderModule;
}

VkShaderStageFlagBits CVulkanShader::GetShaderStageFlagBits()
{
	return this->m_ShaderStageFlagBits;
}