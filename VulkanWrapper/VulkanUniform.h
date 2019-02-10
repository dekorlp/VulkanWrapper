#pragma once

#include "VulkanUniform.h"
#include "vulkan/vulkan.h"
#include <vector>

class CVulkanUniform
{
public:
	void CreateUniform(unsigned int uniformSet, unsigned int uniformBinding, VkDescriptorType descriptorTyp, VkShaderStageFlags shaderStage);
	unsigned int GetUniformSet();
	unsigned int GetUniformBinding();
	VkDescriptorType GetDescriptorType();
	VkShaderStageFlags GetShaderStageFlag();

	void SetUniformBuffers(std::vector<VkBuffer> uniformBuffer, std::vector<VkDeviceMemory> uniformBuffersMemory);
	std::vector<VkBuffer> GetUniformBuffers();
	std::vector<VkDeviceMemory> GetUniformBuffersMemory();

private:
	unsigned int mUniformSet;
	unsigned int mUniformBinding;
	VkDescriptorType mDescriptorType; // exmpl: VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER
	VkShaderStageFlags mShaderStage; // exmpl: VK_SHADER_STAGE_VERTEX_BIT;

	std::vector<VkBuffer> m_UniformBuffers;
	std::vector<VkDeviceMemory> m_UniformBuffersMemory;
};