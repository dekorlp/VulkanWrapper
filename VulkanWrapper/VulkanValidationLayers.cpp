#include "VulkanValidationLayers.h"

bool CVulkanValidationLayers::CheckValidationLayerSupport()
{
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (const char* layerName : validationLayers) {
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


const std::vector<const char*> CVulkanValidationLayers::GetValidationLayers()
{
	return validationLayers;
}

std::vector<const char*> CVulkanValidationLayers::GetRequiredExtensions(uint32_t sdlExtensionCount, const char** sdlExtensions, bool enableValidationLayers)
{
	std::vector<const char*> extensions(sdlExtensions, sdlExtensions + sdlExtensionCount);

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	return extensions;
}