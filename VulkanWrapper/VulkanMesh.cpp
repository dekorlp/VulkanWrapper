#include "VulkanMesh.h"

void CVulkanMesh::Init(CVulkanInstance* instance, CVulkanPipeline* pipeline)
{
	m_Instance = instance;
	m_Pipeline = pipeline;
}

void CVulkanMesh::CreateSecondaryCommandBuffers()
{

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_Instance->GetCommandPool();
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
		allocInfo.commandBufferCount = 1;

		if (vkAllocateCommandBuffers(m_Instance->GetLogicalDevice(), &allocInfo, &m_SecondaryCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate command buffers!");
		}

		VkCommandBufferInheritanceInfo inheritanceInfo;
		inheritanceInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;
		inheritanceInfo.pNext = nullptr;
		inheritanceInfo.renderPass = m_Instance->GetRenderPass();
		inheritanceInfo.subpass = 0;
		inheritanceInfo.occlusionQueryEnable = VK_FALSE;
		inheritanceInfo.framebuffer = VK_NULL_HANDLE;
		inheritanceInfo.pipelineStatistics = 0;

		VkViewport viewport;
		viewport.height = static_cast<float>(m_Instance->GetSwapchainExtend().height);
		viewport.width = static_cast<float>(m_Instance->GetSwapchainExtend().width);
		viewport.minDepth = (float)0.0f;
		viewport.maxDepth = (float)1.0f;
		viewport.x = 0;
		viewport.y = 0;

		VkRect2D scissor;
		scissor.extent.width = m_Instance->GetSwapchainExtend().width;
		scissor.extent.height = m_Instance->GetSwapchainExtend().height;
		scissor.offset.x = 0;
		scissor.offset.y = 0;


		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT | VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;
		beginInfo.pInheritanceInfo = &inheritanceInfo; // Optional

		if (vkBeginCommandBuffer(m_SecondaryCommandBuffer, &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}

		vkCmdBindPipeline(m_SecondaryCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetGraphicsPipeline());

		VkBuffer vertexBuffers[] = { vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers(m_SecondaryCommandBuffer, 0, 1, vertexBuffers, offsets);


		vkCmdSetViewport(m_SecondaryCommandBuffer, 0, 1, &viewport);
		vkCmdSetScissor(m_SecondaryCommandBuffer, 0, 1, &scissor);

		vkCmdBindIndexBuffer(m_SecondaryCommandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT16);
		for (unsigned int i = 0; i < m_DescriptorSets.size(); i++)
		{
			for (size_t j = 0; j < m_Instance->GetSwapchainImages().size(); j++)
			{
				vkCmdBindDescriptorSets(m_SecondaryCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_Pipeline->GetPipelineLayout(), 0, 1, &m_DescriptorSets[i][j], 0, nullptr);
			}
		}

		vkCmdDrawIndexed(m_SecondaryCommandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);

		if (vkEndCommandBuffer(m_SecondaryCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

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
	CVulkanQueueFamily::copyBuffer(m_Instance->GetLogicalDevice(), m_Instance->GetCommandPool(), m_Instance->GetGraphicsQueue(), stagingBuffer, vertexBuffer, bufferSize);

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

	CVulkanQueueFamily::copyBuffer(m_Instance->GetLogicalDevice(), m_Instance->GetCommandPool(), m_Instance->GetGraphicsQueue(), stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(m_Instance->GetLogicalDevice(), stagingBuffer, nullptr);
	vkFreeMemory(m_Instance->GetLogicalDevice(), stagingBufferMemory, nullptr);
}

void CVulkanMesh::CreateUniformBuffer(CVulkanUniform* uniform, size_t uniformBufferSize)
{
	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

	uniformBuffers.resize(m_Instance->GetSwapchainImages().size());
	uniformBuffersMemory.resize(m_Instance->GetSwapchainImages().size());

	for (size_t i = 0; i < m_Instance->GetSwapchainImages().size(); i++) {
		CVulkanQueueFamily::createBuffer(m_Instance->GetPhysicalDevice(), m_Instance->GetLogicalDevice(), uniformBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, uniformBuffers[i], uniformBuffersMemory[i]);
	}
	uniform->SetUniformBuffers(uniformBuffers, uniformBuffersMemory);
}

void CVulkanMesh::CreateDescriptorPool()
{
	for (unsigned int i = 0; i < m_Pipeline->GetDescriptorUniforms().size(); i++)
	{
		std::vector<VkDescriptorPoolSize> descriptorPools;

		for (unsigned int j = 0; j < m_Pipeline->GetDescriptorUniforms()[i].size(); j++)
		{
			VkDescriptorPoolSize poolSize = {};
			poolSize.type = m_Pipeline->GetDescriptorUniforms()[i][j].GetDescriptorType();
			poolSize.descriptorCount = static_cast<uint32_t>(m_Instance->GetSwapchainImages().size());
			descriptorPools.push_back(poolSize);
		}

		VkDescriptorPool descriptorPool;
		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = descriptorPools.size();
		poolInfo.pPoolSizes = descriptorPools.data();
		poolInfo.maxSets = static_cast<uint32_t>(m_Instance->GetSwapchainImages().size());

		if (vkCreateDescriptorPool(m_Instance->GetLogicalDevice(), &poolInfo, nullptr, &descriptorPool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
		m_DescriptorPool.push_back(descriptorPool);
	}
}

void CVulkanMesh::CreateDescriptorSet()
{
	for (unsigned int i = 0; i < m_DescriptorPool.size(); i++)
	{
		std::vector<VkDescriptorSetLayout> layouts(m_Instance->GetSwapchainImages().size(), m_Pipeline->GetDescriptorSetLayouts()[i]);
		VkDescriptorSetAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_DescriptorPool[i];
		allocInfo.descriptorSetCount = static_cast<uint32_t>(m_Instance->GetSwapchainImages().size());
		allocInfo.pSetLayouts = layouts.data();

		std::vector<VkDescriptorSet> descriptorSets;
		descriptorSets.resize(m_Instance->GetSwapchainImages().size());
		if (vkAllocateDescriptorSets(m_Instance->GetLogicalDevice(), &allocInfo, descriptorSets.data()) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		this->m_DescriptorSets.push_back(descriptorSets);
	}
}

void CVulkanMesh::CreateDescriptorWrite(CVulkanUniform* uniform, size_t uniformBufferSize)
{
	for (size_t i = 0; i < m_Instance->GetSwapchainImages().size(); i++) {
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = uniform->GetUniformBuffers()[i];
		bufferInfo.offset = 0;
		bufferInfo.range = uniformBufferSize;

		VkWriteDescriptorSet descriptorWrite = {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_DescriptorSets[uniform->GetUniformSet()][i];
		descriptorWrite.dstBinding = uniform->GetUniformBinding();
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = uniform->GetDescriptorType();
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = nullptr; // Optional
		descriptorWrite.pTexelBufferView = nullptr; // Optional
		vkUpdateDescriptorSets(m_Instance->GetLogicalDevice(), 1, &descriptorWrite, 0, nullptr);
	}
}

void CVulkanMesh::DestroyDescriptorPool()
{
	for (unsigned int i = 0; i < m_DescriptorPool.size(); i++)
	{
		vkDestroyDescriptorPool(m_Instance->GetLogicalDevice(), m_DescriptorPool[i], nullptr);
	}
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

void CVulkanMesh::DestroyUniformBuffers(CVulkanUniform* uniform)
{
	for (size_t i = 0; i < m_Instance->GetSwapchainImages().size(); i++) {
		vkDestroyBuffer(m_Instance->GetLogicalDevice(), uniform->GetUniformBuffers()[i], nullptr);
		vkFreeMemory(m_Instance->GetLogicalDevice(), uniform->GetUniformBuffersMemory()[i], nullptr);
	}
}

VkCommandBuffer* const CVulkanMesh::GetCommandBuffer()
{
	return &m_SecondaryCommandBuffer;
}

void CVulkanMesh::DestroySecondaryCommandBuffer()
{
	vkFreeCommandBuffers(m_Instance->GetLogicalDevice(), m_Instance->GetCommandPool(), static_cast<uint32_t>(1), &m_SecondaryCommandBuffer);
}


void CVulkanMesh::UpdateUniformBuffer(CVulkanUniform uniform, const void* uniformBuffer, size_t uniformBufferSize)
{
	void* data;
	
	vkMapMemory(m_Instance->GetLogicalDevice(), uniform.GetUniformBuffersMemory()[m_Instance->GetSelectedImageToDraw()], 0, uniformBufferSize, 0, &data);
	memcpy(data, uniformBuffer, uniformBufferSize);
	vkUnmapMemory(m_Instance->GetLogicalDevice(), uniform.GetUniformBuffersMemory()[m_Instance->GetSelectedImageToDraw()]);
}

void CVulkanMesh::SetCurrentImage(uint32_t currentImage)
{
	this->m_CurrentImage = currentImage;
}

void CVulkanMesh::SetVectorIndex(unsigned int vectorIndex)
{
	this->m_VectorIndex = vectorIndex;
}

unsigned int CVulkanMesh::GetVectorIndex() const
{
	return this->m_VectorIndex;
}

bool CVulkanMesh::operator==(const CVulkanMesh& rhs) const
{
	const CVulkanMesh* ptr = dynamic_cast<const CVulkanMesh*>(&rhs);
	if (this->m_VectorIndex != ptr->m_VectorIndex)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool CVulkanMesh::operator==(const CVulkanMesh* rhs) const
{
	if (this->m_VectorIndex != rhs->m_VectorIndex)
	{
		return false;
	}
	else
	{
		return true;
	}
}