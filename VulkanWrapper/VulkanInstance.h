#pragma once
#include <string>
#include <vulkan/vulkan.h>
//#include <vector>

#include "VulkanValidationLayers.h"

struct SVulkanExtensions
{
	const std::vector<const char*> InstanceExtensions = {

	};
	const std::vector<const char*> logicalDeviceExtensions = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};
};

class CVulkanInstance
{
public:
	VkInstance CreateVulkanInstance(std::string applicationName, uint32_t applicationVersion, uint32_t vulkanVersion, uint32_t sdlExtensionCount, const char** sdlExtensions);
	void destroyVulkanInstance();
	std::vector<VkExtensionProperties> getExtensions();
	std::vector<const char*> getExtensionsStr();
	VkInstance GetInstance();
	const bool IsvalidationLayerEnabled();
	const std::vector<const char*> GetValidationLayers();

private: 
	VkInstance instance;
	std::vector<VkExtensionProperties> extensions;
	std::vector<const char*> extensionStr;
	CVulkanValidationLayers validationLayers;
	bool areValidationLayersEnables;

	
};