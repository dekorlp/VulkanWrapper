#pragma once

#include <vector>
#include "VulkanVertex.h"
#include "vulkanqueuefamilie.h"
#include "VulkanPipeline.h"
#include "VulkanLogicalDevice.h"

class CVulkanMesh
{
public:
	void Init(CVulkanPhysicalDevice physicalDevice, CVulkanLogicalDevice logicalDevice, CVulkanPresentation presentation, VkCommandPool commandPool);
	void CreateVertexBuffer(std::vector<SVulkanVertex> vertices);
	void DestroyVertexBuffer();
	void createIndexBuffer(const std::vector<uint16_t> indices);
	void DestroyIndexBuffer();
	void CreateSecondaryCommandBuffers(CVulkanPresentation presentation, CVulkanPipeline pipeline, std::vector<VkFramebuffer> framebuffer);
	VkCommandBuffer* const GetCommandBuffer();
	void DestroySecondaryCommandBuffer();

	bool operator==(const CVulkanMesh& rhs) const;

private:
	CVulkanPresentation m_Presentation;
	CVulkanPhysicalDevice m_PhysicalDevice;
	CVulkanLogicalDevice m_LogicalDevice;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	std::vector<SVulkanVertex> m_Vertices;
	std::vector<uint16_t> m_Indices;

	VkCommandBuffer m_SecondaryCommandBuffer;

	VkCommandPool m_CommandPool;
};