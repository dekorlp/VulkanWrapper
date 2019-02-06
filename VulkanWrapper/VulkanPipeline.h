#pragma once

#include <vector>
#include <vulkan\vulkan.h>
#include "VulkanVertex.h"

#include "VulkanLogicalDevice.h"

class CVulkanPipeline
{
public:
	void InitVulkanPipeline(CVulkanInstance* instance);
	void CreateGraphicsPipeline(std::vector<char> vertexShader, std::vector<char> fragmentShader, CVulkanVertex vertex);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	void CreateDescriptorSetLayout();
	void DestroyPipeline();
	void DestroyDescriptorSetLayout();
	VkPipeline GetGraphicsPipeline();
	VkPipelineLayout GetPipelineLayout();
	VkDescriptorSetLayout GetDescriptorSetLayout();

private:
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	CVulkanInstance* m_Instance;
};