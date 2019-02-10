#pragma once

#include <vector>
//#include "VulkanVertex.h"
#include "CustomVertex.h"
#include "vulkanqueuefamilie.h"
#include "VulkanPipeline.h"
#include "VulkanLogicalDevice.h"

#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <chrono>

class CVulkanMesh
{
public:
	void Init(CVulkanInstance* instance, CVulkanPipeline* pipeline);
	void CreateVertexBuffer(std::vector<CCustomVertex> vertices);
	void DestroyVertexBuffer();
	void createIndexBuffer(const std::vector<uint16_t> indices);
	void CreateUniformBuffer(CVulkanUniform* uniform, size_t uniformBufferSize);
	void CreateDescriptorPool();
	void CreateDescriptorSet();
	void CreateDescriptorWrite(CVulkanUniform* uniform, size_t uniformBufferSize);
	void DestroyIndexBuffer();
	void CreateSecondaryCommandBuffers();
	VkCommandBuffer* const GetCommandBuffer();
	void DestroySecondaryCommandBuffer();
	void DestroyUniformBuffers(CVulkanUniform* uniform);
	void DestroyDescriptorPool();
	void UpdateUniformBuffer(CVulkanUniform uniform, const void* uniformBuffer, size_t uniformBufferSize);

	bool operator==(const CVulkanMesh& rhs) const;

	bool operator==(const CVulkanMesh* rhs) const;


	void SetCurrentImage(uint32_t currentImage);
	void SetVectorIndex(unsigned int vectorIndex);
	unsigned int GetVectorIndex() const;

private:
	CVulkanInstance* m_Instance;
	CVulkanPipeline* m_Pipeline;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	unsigned int m_VectorIndex;

	std::vector<VkDescriptorPool> m_DescriptorPool;
	std::vector<std::vector<VkDescriptorSet>> m_DescriptorSets;

	std::vector<CCustomVertex> m_Vertices;
	std::vector<uint16_t> m_Indices;

	VkCommandBuffer m_SecondaryCommandBuffer;

	uint32_t m_CurrentImage = 0;
};