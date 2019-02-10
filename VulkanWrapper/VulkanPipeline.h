#pragma once

#include <vector>
#include <vulkan\vulkan.h>
#include "VulkanVertex.h"

#include "VulkanLogicalDevice.h"
#include "VulkanUniform.h"

class CVulkanPipeline
{
public:
	void InitVulkanPipeline(CVulkanInstance* instance);
	void CreateGraphicsPipeline(std::vector<char> vertexShader, std::vector<char> fragmentShader, CVulkanVertex vertex);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	void CreateDescriptorSetLayouts();
	void DestroyPipeline();
	void DestroyDescriptorSetLayout();
	VkPipeline GetGraphicsPipeline();
	VkPipelineLayout GetPipelineLayout();
	std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts();
	std::vector<std::vector<CVulkanUniform>> GetDescriptorUniforms();
	void AddUniform(CVulkanUniform uniform);

private:
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	CVulkanInstance* m_Instance;
	std::vector<std::vector<CVulkanUniform>> m_UniformSets;
	std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;
};