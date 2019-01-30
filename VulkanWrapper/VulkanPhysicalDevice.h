#pragma once

#include <vector>
#include <map>
#include <set>
#include <vulkan/vulkan.h>
#include "VulkanInstance.h"
#include "VulkanPresentation.h"
#include "VulkanQueueFamilie.h"
#include "vulkanqueuefamilie.h"

class CVulkanPhysicalDevice
{
public:
	void SelectPhysicalDevice(CVulkanInstance* instance, VkSurfaceKHR surface);
	VkPhysicalDevice GetPhysicalDevice();
	bool CheckValidationLayerSupport();
	const std::vector<const char*> GetValidationLayers();
	std::vector<const char*> GetRequiredExtensions(uint32_t sdlExtensionCount, const char** sdlExtensions, bool enableValidationLayers);
	int RateDeviceSuitabilty(VkPhysicalDevice device);
	

private:
	VkInstance m_Instance;
	CVulkanInstance *m_VulkanInstance;
	//CVulkanPresentation m_VulkanPresentation;
	VkPhysicalDevice physicalDevice;
	VkSurfaceKHR m_Surface;
	std::vector<VkLayerProperties> availableLayers;
	//SVulkanExtensions extensions;

	
};