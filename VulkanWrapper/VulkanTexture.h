#pragma once
#include <vulkan\vulkan.h>
#include "VulkanInstance.h"
#include "VulkanUtils.h"

class CVulkanTexture
{
public:
	void CreateTextureImage(CVulkanInstance* instance, unsigned char* pixels, unsigned int width, unsigned int height, unsigned int components);
	void DestroyTexture();
private:
	void CreateImage(CVulkanInstance* instance, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
	void transitionImageLayout(CVulkanInstance* instance, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(CVulkanInstance* instance, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	CVulkanInstance *m_Instance;
};