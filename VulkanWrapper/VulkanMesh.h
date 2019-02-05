#pragma once

#include <vector>
//#include "VulkanVertex.h"
#include "CustomVertex.h"
#include "vulkanqueuefamilie.h"
#include "VulkanPipeline.h"
#include "VulkanLogicalDevice.h"
#include "SUniformBufferObject.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

class CVulkanMesh
{
public:
	void Init(CVulkanPhysicalDevice physicalDevice, CVulkanLogicalDevice logicalDevice, CVulkanPresentation presentation, VkCommandPool commandPool);
	void CreateVertexBuffer(std::vector<CCustomVertex> vertices);
	void DestroyVertexBuffer();
	void createIndexBuffer(const std::vector<uint16_t> indices);
	void CreateUniformBuffer();
	void CreateDescriptorPool();
	void CreateDescriptorSet(CVulkanPipeline pipeline);
	void DestroyIndexBuffer();
	void CreateSecondaryCommandBuffers(CVulkanPresentation presentation, CVulkanPipeline pipeline, std::vector<VkFramebuffer> framebuffer);
	VkCommandBuffer* const GetCommandBuffer();
	void DestroySecondaryCommandBuffer();
	void DestroyUniformBuffers();
	void DestroyDescriptorPool();
	std::vector<VkDeviceMemory> GetUniformBuffersMemory();
	void UpdateUniformBuffers();

	bool operator==(const CVulkanMesh& rhs) const;
	void SetCurrentImage(uint32_t currentImage);

private:
	CVulkanPresentation m_Presentation;
	CVulkanPhysicalDevice m_PhysicalDevice;
	CVulkanLogicalDevice m_LogicalDevice;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	std::vector<VkBuffer> uniformBuffers;
	std::vector<VkDeviceMemory> uniformBuffersMemory;

	VkDescriptorPool descriptorPool;
	std::vector<VkDescriptorSet> descriptorSets;

	std::vector<CCustomVertex> m_Vertices;
	std::vector<uint16_t> m_Indices;

	VkCommandBuffer m_SecondaryCommandBuffer;

	VkCommandPool m_CommandPool;

	UniformBufferObject ubo = {};

	uint32_t m_CurrentImage = 0;
};