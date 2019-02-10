#include "VulkanUniform.h"

void CVulkanUniform::CreateUniform(unsigned int uniformSet, unsigned int uniformBinding, VkDescriptorType descriptorTyp, VkShaderStageFlags shaderStage)
{
	this->mUniformSet = uniformSet;
	this->mUniformBinding = uniformBinding;
	this->mDescriptorType = descriptorTyp;
	this->mShaderStage = shaderStage;
}

void CVulkanUniform::SetUniformBuffers(std::vector<VkBuffer> uniformBuffer, std::vector<VkDeviceMemory> uniformBuffersMemory)
{
	this->m_UniformBuffers = uniformBuffer;
	this->m_UniformBuffersMemory = uniformBuffersMemory;
}


std::vector<VkBuffer> CVulkanUniform::GetUniformBuffers()
{
	return this->m_UniformBuffers;
}

std::vector<VkDeviceMemory> CVulkanUniform::GetUniformBuffersMemory()
{
	return this->m_UniformBuffersMemory;
}

unsigned int CVulkanUniform::GetUniformSet()
{
	return this->mUniformSet;
}

unsigned int CVulkanUniform::GetUniformBinding()
{
	return this->mUniformBinding;
}

VkDescriptorType CVulkanUniform::GetDescriptorType()
{
	return this->mDescriptorType;
}

VkShaderStageFlags CVulkanUniform::GetShaderStageFlag()
{
	return this->mShaderStage;
}