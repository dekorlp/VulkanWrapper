#pragma once
#include <vulkan/vulkan.h>
#include <vector>

class CVulkanValidationLayers
{
public:
	bool CheckValidationLayerSupport();
	const std::vector<const char*> GetValidationLayers();
	std::vector<const char*> GetRequiredExtensions(uint32_t sdlExtensionCount, const char** sdlExtensions, bool enableValidationLayers);
private:
	std::vector<VkLayerProperties> availableLayers;
	const std::vector<const char*> validationLayers = {
		"VK_LAYER_LUNARG_standard_validation"
	};
};