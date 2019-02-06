#pragma once
#include <vulkan/vulkan.h>
#include "VulkanInstance.h"
#include <iostream>
#include <vector>

class CVulkanDebug
{
public:
	void SetupVulkanDebug(CVulkanInstance instance);
	void SetupDebugCallback( void* debugCallback);
	VkResult CreateDebugUtilsMessengerEXT (const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pCallback);
	void DestroyDebugUtilsMessengerEXT(const VkAllocationCallbacks* pAllocator);

private:
	
	VkInstance m_Instance;
	std::vector<VkDebugUtilsMessengerEXT> m_DebugHandler;
	bool m_AreValidationLayersEnables;
};