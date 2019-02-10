#pragma once

#include <set>
#include "VulkanInstance.h"
#include "VulkanPhysicalDevice.h"
#include "VulkanUtils.h"
#include "vulkanpresentation.h"

class CVulkanLogicalDevice
{
public:
	void CreateLogicalDevice(CVulkanInstance* instance);
	void DestroyLogicalDevice();
	VkDevice getDevice();
	VkQueue GetGraphicsQueue();
	VkQueue GetPresentQueue();
private:
	//VkDevice m_Device; // LogicalDevice
	//VkQueue graphicsQueue;
	//VkQueue presentQueue;
	CVulkanInstance *m_Instance;
};