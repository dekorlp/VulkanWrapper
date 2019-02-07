#pragma once

#include <vulkan\vulkan.h>

struct SVulkanUniformBufferlayout
{
	VkDescriptorSetLayout layout;
	unsigned int binding;
};