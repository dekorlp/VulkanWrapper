#pragma once

#include <vector>
#include <vulkan\vulkan.h>
#include "VulkanVertex.h"

#include "VulkanLogicalDevice.h"

class CVulkanPipeline
{
public:
	void InitVulkanPipeline(CVulkanLogicalDevice device, CVulkanPresentation presentation);
	void CreateGraphicsPipeline(std::vector<char> vertexShader, std::vector<char> fragmentShader);
	VkShaderModule CreateShaderModule(const std::vector<char>& code);
	void CreateDescriptorSetLayout();
	void DestroyPipeline();
	void DestroyDescriptorSetLayout();
	VkPipeline GetGraphicsPipeline();
	VkPipelineLayout GetPipelineLayout();
	VkDescriptorSetLayout GetDescriptorSetLayout();

private:
	CVulkanLogicalDevice m_LogicalDevice;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	CVulkanPresentation m_Presentation;
	VkPipeline graphicsPipeline;
};