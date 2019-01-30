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
	void DestroyPipeline();
	VkPipeline GetGraphicsPipeline();
	VkPipelineLayout GetPipelineLayout();

private:
	CVulkanLogicalDevice m_LogicalDevice;
	VkPipelineLayout pipelineLayout;
	CVulkanPresentation m_Presentation;
	VkPipeline graphicsPipeline;
};