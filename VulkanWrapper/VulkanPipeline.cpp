#include "VulkanPipeline.h"

void CVulkanPipeline::InitVulkanPipeline(CVulkanInstance* instance)
{
	m_Instance = instance;
}

void CVulkanPipeline::CreateGraphicsPipeline(CVulkanVertex vertex)
{
	auto bindingDesription = vertex.getBindingDescription();
	auto attributeDescription = vertex.getAttributeDescription();

	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDesription.size());
	vertexInputInfo.pVertexBindingDescriptions = bindingDesription.data(); // Optional
	vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescription.size());
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data(); // Optional

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = (float)m_Instance->GetSwapchainExtend().width;  
	viewport.height = (float)m_Instance->GetSwapchainExtend().height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = m_Instance->GetSwapchainExtend();

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; // Optional
	rasterizer.depthBiasClamp = 0.0f; // Optional
	rasterizer.depthBiasSlopeFactor = 0.0f; // Optional

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; // Optional
	multisampling.pSampleMask = nullptr; // Optional
	multisampling.alphaToCoverageEnable = VK_FALSE; // Optional
	multisampling.alphaToOneEnable = VK_FALSE; // Optional

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_FALSE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = m_DescriptorSetLayouts.size(); // Optional
	pipelineLayoutInfo.pSetLayouts = m_DescriptorSetLayouts.data(); // Optional
	pipelineLayoutInfo.pushConstantRangeCount = 0; // Optional
	pipelineLayoutInfo.pPushConstantRanges = nullptr; // Optional



	if (vkCreatePipelineLayout(m_Instance->GetLogicalDevice(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkDynamicState dynamicStateEnables[VK_DYNAMIC_STATE_RANGE_SIZE];
	VkPipelineDynamicStateCreateInfo dynamicState = {};
	memset(dynamicStateEnables, 0, sizeof dynamicStateEnables);
	dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicState.pNext = NULL;
	dynamicState.pDynamicStates = dynamicStateEnables;
	dynamicState.dynamicStateCount = 0;
	dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
	dynamicStateEnables[dynamicState.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;


	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = m_ShaderStages.size();
	pipelineInfo.pStages = m_ShaderStages.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = nullptr; // Optional
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = &dynamicState; // Optional
	pipelineInfo.layout = pipelineLayout;
	pipelineInfo.renderPass = m_Instance->GetRenderPass();
	pipelineInfo.subpass = 0;
	
	if (vkCreateGraphicsPipelines(m_Instance->GetLogicalDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	//shaderModule is not longer needed
	for (unsigned int i = 0; i < m_VulkanShader.size(); i++)
	{
		vkDestroyShaderModule(m_Instance->GetLogicalDevice(), m_VulkanShader[i].GetShaderModule(), nullptr);
	}

}

void CVulkanPipeline::CreateDescriptorSetLayouts()
{
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
	m_DescriptorSetLayouts.resize(m_UniformSets.size());


	for (unsigned int i = 0; i < m_UniformSets.size(); i++)
	{
		for (unsigned int j = 0; j < m_UniformSets.at(i).size(); j++)
		{
			VkDescriptorSetLayoutBinding uboLayoutBinding = {};
			uboLayoutBinding.binding = m_UniformSets.at(i).at(j).GetUniformBinding();
			uboLayoutBinding.descriptorType = m_UniformSets.at(i).at(j).GetDescriptorType();
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.stageFlags = m_UniformSets.at(i).at(j).GetShaderStageFlag();
			uboLayoutBinding.pImmutableSamplers = nullptr; // Optional
			layoutBindings.push_back(uboLayoutBinding);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = layoutBindings.size();
		layoutInfo.pBindings = layoutBindings.data();

		if (vkCreateDescriptorSetLayout(m_Instance->GetLogicalDevice(), &layoutInfo, nullptr, &m_DescriptorSetLayouts[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
		layoutBindings.clear();
	}
}

std::vector<std::vector<CVulkanUniform>> CVulkanPipeline::GetDescriptorUniforms()
{
	return m_UniformSets;
}

void CVulkanPipeline::AddUniform(CVulkanUniform uniform)
{
	if (uniform.GetUniformSet() > m_UniformSets.size())
	{
		throw std::runtime_error("failed to add Uniform! Uniform sets must be crated in an ascending order starting with 0!");
	}
	else if (uniform.GetUniformSet() == m_UniformSets.size())
	{
		m_UniformSets.push_back(std::vector<CVulkanUniform>(1, uniform));
	}
	else // uniform.GetUniformSet() < m_Uniform.size()
	{
		m_UniformSets.at(uniform.GetUniformSet()).push_back(uniform);
	}
}

void CVulkanPipeline::AddShader(std::vector<char> shaderProgram, VkShaderStageFlagBits shaderStage)
{
	
	VkShaderModule shaderModule = CVulkanQueueFamily::CreateShaderModule(m_Instance, shaderProgram);

	VkPipelineShaderStageCreateInfo shaderStageInfo = {};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = shaderStage;
	shaderStageInfo.module = shaderModule;
	shaderStageInfo.pName = "main";

	m_ShaderStages.push_back(shaderStageInfo);

	CVulkanShader shader;
	shader.SetShaderModule(shaderModule);
	shader.SetShaderStageFlagBits(shaderStage);

	m_VulkanShader.push_back(shader);
}

void CVulkanPipeline::DestroyPipeline()
{
	vkDestroyPipeline(m_Instance->GetLogicalDevice(), graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_Instance->GetLogicalDevice(), pipelineLayout, nullptr);
	
}

void CVulkanPipeline::DestroyDescriptorSetLayout()
{
	for (unsigned int i = 0; i < m_DescriptorSetLayouts.size(); i++)
	{
		vkDestroyDescriptorSetLayout(m_Instance->GetLogicalDevice(), m_DescriptorSetLayouts[i], nullptr);
	}
}

VkPipeline CVulkanPipeline::GetGraphicsPipeline()
{
	return graphicsPipeline;
}

VkPipelineLayout CVulkanPipeline::GetPipelineLayout()
{
	return pipelineLayout;
}

std::vector<VkDescriptorSetLayout> CVulkanPipeline::GetDescriptorSetLayouts()
{
	return m_DescriptorSetLayouts;
}