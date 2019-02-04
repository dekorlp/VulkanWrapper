#include "VulkanMesh.h"

void CVulkanMesh::Init(CVulkanPhysicalDevice physicalDevice, CVulkanLogicalDevice logicalDevice, CVulkanPresentation presentation, VkCommandPool commandPool)
{
	m_PhysicalDevice = physicalDevice;
	m_LogicalDevice = logicalDevice;
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

		if (vkAllocateCommandBuffers(m_LogicalDevice.getDevice(), &allocInfo, &m_SecondaryCommandBuffer) != VK_SUCCESS) {
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

		//vkCmdDraw(commandBuffers[i], static_cast<uint32_t>(vertices.size()), 1, 0, 0);
		vkCmdDrawIndexed(m_SecondaryCommandBuffer, static_cast<uint32_t>(m_Indices.size()), 1, 0, 0, 0);

		if (vkEndCommandBuffer(m_SecondaryCommandBuffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}

		//m_SecondaryCommandBuffer.push_back(secondaryCommandBuffer);
}

void CVulkanMesh::CreateVertexBuffer(const std::vector<SVulkanVertex> vertices)
{
	m_Vertices = vertices;

	VkDeviceSize bufferSize = sizeof(m_Vertices[0]) * m_Vertices.size();
	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;

	CVulkanQueueFamily::createBuffer(m_PhysicalDevice.GetPhysicalDevice(), m_LogicalDevice.getDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_LogicalDevice.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Vertices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_LogicalDevice.getDevice(), stagingBufferMemory);

	CVulkanQueueFamily::createBuffer(m_PhysicalDevice.GetPhysicalDevice(), m_LogicalDevice.getDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, vertexBuffer, vertexBufferMemory);
	CVulkanQueueFamily::copyBuffer(m_LogicalDevice.getDevice(), m_CommandPool, m_LogicalDevice.GetGraphicsQueue(), stagingBuffer, vertexBuffer, bufferSize);

	vkDestroyBuffer(m_LogicalDevice.getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(m_LogicalDevice.getDevice(), stagingBufferMemory, nullptr);
}

void CVulkanMesh::createIndexBuffer(const std::vector<uint16_t> indices) {
	m_Indices = indices;

	VkDeviceSize bufferSize = sizeof(m_Indices[0]) * m_Indices.size();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CVulkanQueueFamily::createBuffer(m_PhysicalDevice.GetPhysicalDevice(), m_LogicalDevice.getDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

	void* data;
	vkMapMemory(m_LogicalDevice.getDevice(), stagingBufferMemory, 0, bufferSize, 0, &data);
	memcpy(data, m_Indices.data(), (size_t)bufferSize);
	vkUnmapMemory(m_LogicalDevice.getDevice(), stagingBufferMemory);

	CVulkanQueueFamily::createBuffer(m_PhysicalDevice.GetPhysicalDevice(), m_LogicalDevice.getDevice(), bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, indexBuffer, indexBufferMemory);

	CVulkanQueueFamily::copyBuffer(m_LogicalDevice.getDevice(), m_CommandPool, m_LogicalDevice.GetGraphicsQueue(), stagingBuffer, indexBuffer, bufferSize);

	vkDestroyBuffer(m_LogicalDevice.getDevice(), stagingBuffer, nullptr);
	vkFreeMemory(m_LogicalDevice.getDevice(), stagingBufferMemory, nullptr);
}

void CVulkanMesh::DestroyVertexBuffer()
{
	vkDestroyBuffer(m_LogicalDevice.getDevice(), vertexBuffer, nullptr);
	vkFreeMemory(m_LogicalDevice.getDevice(), vertexBufferMemory, nullptr);
}

void CVulkanMesh::DestroyIndexBuffer()
{
	vkDestroyBuffer(m_LogicalDevice.getDevice(), indexBuffer, nullptr);
	vkFreeMemory(m_LogicalDevice.getDevice(), indexBufferMemory, nullptr);
}

VkCommandBuffer* const CVulkanMesh::GetCommandBuffer()
{
	return &m_SecondaryCommandBuffer;
}

void CVulkanMesh::DestroySecondaryCommandBuffer()
{
	vkFreeCommandBuffers(m_LogicalDevice.getDevice(), m_CommandPool, static_cast<uint32_t>(1), &m_SecondaryCommandBuffer);
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