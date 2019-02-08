#include "VulkanLogicalDevice.h"
#include <vulkan/vulkan.h>

void CVulkanLogicalDevice::CreateLogicalDevice(CVulkanInstance* instance)
{

	m_Instance = instance;
	SQueueFamilyIndices indices = CVulkanQueueFamily::findQueueFamilies(instance->GetPhysicalDevice(),  instance->GetSurface());

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

	SVulkanExtensions vulkanExtensions;
	createInfo.enabledExtensionCount = static_cast<uint32_t>(vulkanExtensions.logicalDeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = vulkanExtensions.logicalDeviceExtensions.data();

	const std::vector<const char*> validationLayersVector = instance->GetValidationLayers();
	if (instance->IsvalidationLayerEnabled()) {
		
		createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayersVector.size());

		createInfo.ppEnabledLayerNames = validationLayersVector.data();
	}
	else {
		createInfo.enabledLayerCount = 0;
	}

	VkDevice device;
	if (vkCreateDevice(instance->GetPhysicalDevice(), &createInfo, nullptr, &device) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	VkQueue graphicsQueue;
	VkQueue presentQueue;

	vkGetDeviceQueue(device, indices.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(device, indices.presentFamily, 0, &presentQueue);

	m_Instance->SetPresentQueue(presentQueue);
	m_Instance->SetGraphicsQueue(graphicsQueue);
	m_Instance->SetLogicalDevice(device);

}

void CVulkanLogicalDevice::DestroyLogicalDevice()
{
	vkDestroyDevice(m_Instance->GetLogicalDevice(), nullptr);
}

VkDevice CVulkanLogicalDevice::getDevice()
{
	return m_Instance->GetLogicalDevice();
}

VkQueue CVulkanLogicalDevice::GetGraphicsQueue()
{
	return m_Instance->GetGraphicsQueue();
}

VkQueue CVulkanLogicalDevice::GetPresentQueue()
{
	return m_Instance->GetPresentQueue();
}