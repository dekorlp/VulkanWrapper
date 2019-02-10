#pragma once

#include <vector>
#include <vulkan\vulkan.h>
#include "VulkanVertex.h"

#include "VulkanLogicalDevice.h"
#include "VulkanUniform.h"
#include "CVulkanShader.h"

class CVulkanPipeline
{
public:
	void InitVulkanPipeline(CVulkanInstance* instance);
	void CreateGraphicsPipeline(CVulkanVertex vertex);
	void CreateDescriptorSetLayouts();
	void DestroyPipeline();
	void DestroyDescriptorSetLayout();
	VkPipeline GetGraphicsPipeline();
	VkPipelineLayout GetPipelineLayout();
	std::vector<VkDescriptorSetLayout> GetDescriptorSetLayouts();
	std::vector<std::vector<CVulkanUniform>> GetDescriptorUniforms();
	void AddUniform(CVulkanUniform uniform);
	void AddShader(std::vector<char> shaderProgram, VkShaderStageFlagBits shaderStage);
	void AddShader(const char* shaderProgram, unsigned int shaderProgramSize, VkShaderStageFlagBits shaderStage);

private:
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;

	CVulkanInstance* m_Instance;
	std::vector<std::vector<CVulkanUniform>> m_UniformSets;
	std::vector<VkDescriptorSetLayout> m_DescriptorSetLayouts;

	std::vector<CVulkanShader> m_VulkanShader;
	std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStages;
};