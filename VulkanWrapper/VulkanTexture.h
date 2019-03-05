#pragma once
#include <vulkan\vulkan.h>
#include "VulkanInstance.h"
#include "VulkanUtils.h"

class CVulkanTexture
{
public:
	void CreateTextureImage(CVulkanInstance instance, unsigned char* pixels, unsigned int width, unsigned int height, unsigned int components);
private:
	void CreateImage(CVulkanInstance instance, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
};