#include "VulkanPhysicalDevice.h"

void CVulkanPhysicalDevice::SelectPhysicalDevice(CVulkanInstance* instance)
{

	//m_VulkanInstance = instance;
	//m_Surface = surface;
	//m_VulkanPresentation = presentation;

	physicalDevice = VK_NULL_HANDLE;

	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance->GetInstance(), &deviceCount, nullptr);

	if (deviceCount == 0)
	{
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}

	

	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(instance->GetInstance(), &deviceCount, devices.data());

	//for (const auto& device : devices) {
	//	if (IsDeviceSuitable(device)) {
	//		physicalDevice = device;
	//		break;
	//	}
	//}

	//if (physicalDevice == VK_NULL_HANDLE) {
	//	throw std::runtime_error("failed to find a suitable GPU!");
	//}

	std::multimap<int, VkPhysicalDevice> candidates;

	for (const auto& device : devices) {
		int score = RateDeviceSuitabilty(device, instance->GetSurface());
		candidates.insert(std::make_pair(score, device));
	}

	// Check if the best candidate is suitable at all
	if (candidates.rbegin()->first > 0) {
		physicalDevice = candidates.rbegin()->second;
		instance->SetPhysicalDevice(candidates.rbegin()->second);
	}
	else {
		throw std::runtime_error("failed to find a suitable GPU!");
	}
}

VkPhysicalDevice CVulkanPhysicalDevice::GetPhysicalDevice()
{
	return physicalDevice;
}

bool CVulkanPhysicalDevice::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
	SVulkanExtensions extensions;
	for (const char* layerName : extensions.validationLayers) {
		bool layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}


const std::vector<const char*> CVulkanPhysicalDevice::GetValidationLayers()
{
	SVulkanExtensions extensions;
	return extensions.validationLayers;
}

std::vector<const char*> CVulkanPhysicalDevice::GetRequiredExtensions(uint32_t sdlExtensionCount, const char** sdlExtensions, bool enableValidationLayers)
{
	std::vector<const char*> extensions(sdlExtensions, sdlExtensions + sdlExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}

// TODO: Implement DeviceRating for choosing the best device
int CVulkanPhysicalDevice::RateDeviceSuitabilty(VkPhysicalDevice device, VkSurfaceKHR surface)
{
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);

	int score = 0;

	// Discrete GPUs have a significant performance advantage
	if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
		score += 1000;
	}

	// Maximum possible size of textures affects graphics quality
	score += deviceProperties.limits.maxImageDimension2D;

	// Application can't function without geometry shaders
	if (!deviceFeatures.geometryShader) {
		return 0;
	}

	// check if the device contains a graphics queue
	SQueueFamilyIndices indices = CVulkanUtils::findQueueFamilies(device, surface);
	if (indices.isComplete() == 0)
	{
		score = 0;
	}

	return score;
}