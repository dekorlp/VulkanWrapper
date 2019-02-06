#pragma once

#include <vector>
#include <vulkan/vulkan.h>
#include "VulkanInstance.h"
#include "VulkanLogicalDevice.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanPipeline.h"
#include "vulkanpresentation.h"
#include "vulkanqueuefamilie.h"
#include "VulkanMesh.h"

class CVulkanDrawing
{
public:
	void Init(CVulkanInstance* instance);
	void CreateFrameBuffers();
	void DestroyFrameBuffers();
	void CreateCommandPool();
	void DestroyCommandPool();
	void CreateCommandBuffers();
	void DestroyCommandBuffers();
	void CreateSemaphores();
	void DestroySemaphores();
	void RegisterMesh(CVulkanMesh commandBuffer);
	void UnregisterMesh(CVulkanMesh commandBuffer);
	void Draw();
	void CreateSyncObjects();
	std::vector<VkFramebuffer> getFramebuffers();
	uint32_t GetCurrentImageToDraw();

	VkCommandPool GetCommandPool();

private:
	//std::vector<VkFramebuffer> swapChainFramebuffers;
	//CVulkanLogicalDevice m_LogicalDevice;
	//CVulkanPipeline m_Pipeline;
	//CVulkanPresentation m_Presentation;
	//CVulkanPhysicalDevice m_PhysicalDevice;
	CVulkanInstance* m_Instance;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	VkSemaphore imageAvailableSemaphore;
	VkSemaphore renderFinishedSemaphore;

	const unsigned int MAX_FRAMES_IN_FLIGHT = 2;
	std::vector<VkSemaphore> imageAvailableSemaphores;
	std::vector<VkSemaphore> renderFinishedSemaphores;
	size_t currentFrame = 0;

	std::vector<VkFence> inFlightFences;

	std::vector<CVulkanMesh> m_VulkanMesh;

	//uint32_t m_CurrentImageToDraw;
};