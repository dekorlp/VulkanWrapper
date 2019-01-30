#pragma once

#include <set>
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanQueueFamilie.h"
#include "vulkanpresentation.h"

class CVulkanLogicalDevice
{
public:
	void CreateLogicalDevice(CVulkanInstance instance, CVulkanPhysicalDevice physicalDevice, CVulkanPresentation presentation);
	void DestroyLogicalDevice();
	VkDevice getDevice();
	VkQueue GetGraphicsQueue();
	VkQueue GetPresentQueue();
private:
	VkDevice m_Device; // LogicalDevice
	VkQueue graphicsQueue;
	VkQueue presentQueue;
};