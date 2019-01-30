#include "VulkanLogicalDevice.h"
#include <vulkan/vulkan.h>

void CVulkanLogicalDevice::CreateLogicalDevice(CVulkanInstance instance, CVulkanPhysicalDevice physicalDevice, CVulkanPresentation presentation)
{
	//VulkanQueueFamily queueFamily;
	SQueueFamilyIndices indices = CVulkanQueueFamily::findQueueFamilies(physicalDevice.GetPhysicalDevice(),  presentation.GetSurface());

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<uint32_t> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };

	float queuePriority = 1.0f;
	for (uint32_t queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriority;
		queueCreateInfos.push_back(queueCreateInfo);
	}


	VkPhysicalDeviceFeatures deviceFeatures = {};

	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.queueCreateInfoCount = 1;

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = 0;

	//const std::vector<const char*> validationExtensionVector = instance.getExtensionsStr();
	//createInfo.enabledExtensionCount = static_cast<uint32_t>(validationExtensionVector.size());
	//createInfo.ppEnabledExtensionNames = validationExtensionVector.data();

	SVulkanExtensions vulkanExtensions;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(vulkanExtensions.logicalDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = vulkanExtensions.logicalDeviceExtensions.data();

	const std::vector<const char*> validationLayersVector = instance.GetValidationLayers();
	if (instance.IsvalidationLayerEnabled()) {
		
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayersVector.size());

		createInfo.ppEnabledLayerNames = validationLayersVector.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	if (vkCreateDevice(physicalDevice.GetPhysicalDevice(), &createInfo, nullptr, &m_Device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	vkGetDeviceQueue(m_Device, indices.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(m_Device, indices.presentFamily, 0, &presentQueue);

}

void CVulkanLogicalDevice::DestroyLogicalDevice()
{
	vkDestroyDevice(m_Device, nullptr);
}

VkDevice CVulkanLogicalDevice::getDevice()
{
	return m_Device;
}

VkQueue CVulkanLogicalDevice::GetGraphicsQueue()
{
	return graphicsQueue;
}

VkQueue CVulkanLogicalDevice::GetPresentQueue()
{
	return presentQueue;
}