#include "VulkanMesh.h"

void CVulkanMesh::Init(CVulkanInstance* instance, CVulkanPresentation presentation, VkCommandPool commandPool)
{
	m_Instance = instance;
	//m_PhysicalDevice = physicalDevice;
	//m_LogicalDevice = logicalDevice;
	m_Presentation = presentation;
	m_CommandPool = commandPool;
}

void CVulkanMesh::CreateSecondaryCommandBuffers(CVulkanPresentation presentation, CVulkanPipeline pipeline, std::vector<VkFramebuffer> framebuffer)
{

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_CommandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(m_Instance->GetLogicalDevice(), &allocInfo, &m_SecondaryCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}

		VkCommandBufferInheritanceInfo inheritanceInfo;
		inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritanceInfo.pNext = nullptr;
		inheritanceInfo.renderPass = presentation.GetRenderPass();
		inheritanceInfo.subpass = 0;
		inheritanceInfo.occlusionQueryEnable = VK_FALSE;
		inheritanceInfo.framebuffer = VK_NULL_HANDLE;
		inheritanceInfo.pipelineStatistics = 0;

		VkViewport viewport;
		viewport.height = static_cast<float>(m_Presentation.GetSwapChainExtend().width);
		viewport.width = static_cast<float>(m_Presentation.GetSwapChainExtend().height);
		viewport.minDepth = (float)0.0f;
		viewport.maxDepth = (float)1.0f;
		viewport.x = 0;
		viewport.y = 0;

		VkRect2D scissor;
		scissor.extent.width = m_Presentation.GetSwapChainExtend().width;
		scissor.extent.height = m_Presentation.GetSwapChainExtend().height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;


		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		beginInfo.pInheritanceInfo = &inheritanceInfo; // Optional

		if (vkBeginCommandBuffer(m_SecondaryCommandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		vkCmdBindPipeline(m_SecondaryCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetGraphicsPipeline());

		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(m_SecondaryCommandBuffer, 0, 1, vertexBuffers, offsets);


		vkCmdSetViewport(m_SecondaryCommandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(m_SecondaryCommandBuffer, 0, 1, &scissor);

		vkCmdBindIndexBuffer(m_SecondaryCommandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);

		for (size_t i = 0; i < m_Presentation.GetSwapChainImages().size(); i++)
		{
			vkCmdBindDescriptorSets(m_SecondaryCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline.GetPipelineLayout(), 0, 1, &descriptorSets[i], 0, nullptr);
		}

		//vkCmdDraw(commandBuffers[i], static_cast<uint32_t>(vertices.size()), 1, 0, 0);
		vkCmdDrawIndexed(m_SecondaryCommandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);

		if (vkEndCommandBuffer(m_SecondaryCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		//m_SecondaryCommandBuffer.push_back(secondaryCommandBuffer);
}

void CVulkanMesh::CreateVertexBuffer(const std::vector<CCustomVertex> vertices)
{
	m_Vertices = vertices;

	VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	CVulkanQueueFamily::createBuffer(m_Instance->GetPhysicalDevice(), m_Instance->GetLogicalDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_Instance->GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_Instance->GetLogicalDevice(), stagingBufferMemory);

	CVulkanQueueFamily::createBuffer(m_Instance->GetPhysicalDevice(), m_Instance->GetLogicalDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	CVulkanQueueFamily::copyBuffer(m_Instance->GetLogicalDevice(), m_CommandPool, m_Instance->GetGraphicsQueue(), stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(m_Instance->GetLogicalDevice(), stagingBuffer, nullptr);
	vkFreeMemory(m_Instance->GetLogicalDevice(), stagingBufferMemory, nullptr);
}

void CVulkanMesh::createIndexBuffer(const std::vector<uint16_t> indices) {
	m_Indices = indices;

	VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CVulkanQueueFamily::createBuffer(m_Instance->GetPhysicalDevice(), m_Instance->GetLogicalDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_Instance->GetLogicalDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Indices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_Instance->GetLogicalDevice(), stagingBufferMemory);

	CVulkanQueueFamily::createBuffer(m_Instance->GetPhysicalDevice(), m_Instance->GetLogicalDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	CVulkanQueueFamily::copyBuffer(m_Instance->GetLogicalDevice(), m_CommandPool, m_Instance->GetGraphicsQueue(), stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(m_Instance->GetLogicalDevice(), stagingBuffer, nullptr);
	vkFreeMemory(m_Instance->GetLogicalDevice(), stagingBufferMemory, nullptr);
}

void CVulkanMesh::CreateUniformBuffer()
{
	VkDeviceSize bufferSize = sizeof(UniformBufferObject);

	uniformBuffers.resize(m_Presentation.GetSwapChainImages().size());
	uniformBuffersMemory.resize(m_Presentation.GetSwapChainImages().size());

	for (size_t i = 0; i < m_Presentation.GetSwapChainImages().size(); i++) {
		CVulkanQueueFamily::createBuffer(m_Instance->GetPhysicalDevice(), m_Instance->GetLogicalDevice(), bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
	}
}

void CVulkanMesh::CreateDescriptorPool()
{
	VkDescriptorPoolSize poolSize = {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast<uint32_t>(m_Presentation.GetSwapChainImages().size());

	VkDescriptorPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast<uint32_t>(m_Presentation.GetSwapChainImages().size());

	if (vkCreateDescriptorPool(m_Instance->GetLogicalDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create descriptor pool!");
	}
}

void CVulkanMesh::CreateDescriptorSet(CVulkanPipeline pipeline)
{
	std::vector<VkDescriptorSetLayout> layouts(m_Presentation.GetSwapChainImages().size(), pipeline.GetDescriptorSetLayout());
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = descriptorPool;
	allocInfo.descriptorSetCount = static_cast<uint32_t>(m_Presentation.GetSwapChainImages().size());
	allocInfo.pSetLayouts = layouts.data();

	descriptorSets.resize(m_Presentation.GetSwapChainImages().size());
	if (vkAllocateDescriptorSets(m_Instance->GetLogicalDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor sets!");
	}

	for (size_t i = 0; i < m_Presentation.GetSwapChainImages().size(); i++) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniformBuffers[i];
		bufferInfo.offset = 0;
		bufferInfo.range = sizeof(UniformBufferObject);

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = descriptorSets[i];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional
		vkUpdateDescriptorSets(m_Instance->GetLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
	}

}

void CVulkanMesh::DestroyDescriptorPool()
{
	vkDestroyDescriptorPool(m_Instance->GetLogicalDevice(), descriptorPool, nullptr);
}

void CVulkanMesh::DestroyVertexBuffer()
{
	vkDestroyBuffer(m_Instance->GetLogicalDevice(), vertexBuffer, nullptr);
	vkFreeMemory(m_Instance->GetLogicalDevice(), vertexBufferMemory, nullptr);
}

void CVulkanMesh::DestroyIndexBuffer()
{
	vkDestroyBuffer(m_Instance->GetLogicalDevice(), indexBuffer, nullptr);
	vkFreeMemory(m_Instance->GetLogicalDevice(), indexBufferMemory, nullptr);
}

void CVulkanMesh::DestroyUniformBuffers()
{
	for (size_t i = 0; i < m_Presentation.GetSwapChainImages().size(); i++) {
		vkDestroyBuffer(m_Instance->GetLogicalDevice(), uniformBuffers[i], nullptr);
		vkFreeMemory(m_Instance->GetLogicalDevice(), uniformBuffersMemory[i], nullptr);
	}
}

VkCommandBuffer* const CVulkanMesh::GetCommandBuffer()
{
	return &m_SecondaryCommandBuffer;
}

void CVulkanMesh::DestroySecondaryCommandBuffer()
{
	vkFreeCommandBuffers(m_Instance->GetLogicalDevice(), m_CommandPool, static_cast<uint32_t>(1), &m_SecondaryCommandBuffer);
}

std::vector<VkDeviceMemory> CVulkanMesh::GetUniformBuffersMemory()
{
	return uniformBuffersMemory;
}


void CVulkanMesh::UpdateUniformBuffers(uint32_t currentImage)
{
	static auto startTime = std::chrono::high_resolution_clock::now();

	auto currentTime = std::chrono::high_resolution_clock::now();
	float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

	UniformBufferObject ubo = {};
	ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	ubo.view = glm::lookAt(glm::vec3(2.0f, 2.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	ubo.proj = glm::perspective(glm::radians(45.0f), m_Presentation.GetSwapChainExtend().width / (float)m_Presentation.GetSwapChainExtend().height, 0.1f, 10.0f);

	ubo.proj[1][1] *= -1;

	void* data;
	
	vkMapMemory(m_Instance->GetLogicalDevice(), uniformBuffersMemory[currentImage], 0, sizeof(ubo), 0, &data);
	memcpy(data, &ubo, sizeof(ubo));
	vkUnmapMemory(m_Instance->GetLogicalDevice(), uniformBuffersMemory[currentImage]);
}

void CVulkanMesh::SetCurrentImage(uint32_t currentImage)
{
	this->m_CurrentImage = currentImage;
}

bool CVulkanMesh::operator==(const CVulkanMesh& rhs) const
{
	const CVulkanMesh* ptr = dynamic_cast<const CVulkanMesh*>(&rhs);
	if (this != ptr)
	{
		return false;
	}
	else
	{
		return true;
	}
}