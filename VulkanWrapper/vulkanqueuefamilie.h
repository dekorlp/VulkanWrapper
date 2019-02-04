#pragma once

#include <vulkan/vulkan.h>
#include <vector>

#include "VulkanInstance.h"
#include "VulkanPresentation.h"

struct SSwapChainSupportDetails {
	VkSurfaceCapabilitiesKHR capabilities;
	std::vector<VkSurfaceFormatKHR> formats;
	std::vector<VkPresentModeKHR> presentModes;
};

struct SQueueFamilyIndices {
	uint32_t graphicsFamily;
	uint32_t presentFamily;

	bool isComplete() {
		return(graphicsFamily >= 0 && presentFamily >= 0);
	}
};

class CVulkanQueueFamily
{
public:
	static bool IsDeviceSuitable(CVulkanInstance vulkanInstance, VkPhysicalDevice device, VkSurfaceKHR surface);
	static bool checkDeviceExtensionSupport(CVulkanInstance vulkanInstance, VkPhysicalDevice device);
	static SQueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);
	static SSwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);
	static uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);
	static void createBuffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
	static void copyBuffer(VkDevice device, VkCommandPool commandPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};