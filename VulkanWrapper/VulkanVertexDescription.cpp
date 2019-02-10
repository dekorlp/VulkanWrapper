#include "VulkanVertexDescription.h"

void CVulkanVertexDescription::AddVertexInputBindingDescription(VkVertexInputBindingDescription bindingDescription)
{
	this->m_VertexInputBindingDescription.push_back(bindingDescription);
}

void CVulkanVertexDescription::AddVertexInputAttributeDescription(VkVertexInputAttributeDescription attributeDescription)
{
	this->m_VertexInputAttributeDescription.push_back(attributeDescription);
}

void CVulkanVertexDescription::AddVertexInputBindingDescription(size_t SizeOfVertexType, unsigned int binding)
{
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = binding;
	bindingDescription.stride = SizeOfVertexType;
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	m_VertexInputBindingDescription.push_back(bindingDescription);
}

void CVulkanVertexDescription::AddVertexInputAttributeDescription(size_t offset, unsigned int binding, unsigned int location, VkFormat format)
{
	
	VkVertexInputAttributeDescription positionAttributeDescription;
	positionAttributeDescription.binding = binding;
	positionAttributeDescription.location = location;
	positionAttributeDescription.format = format;
	positionAttributeDescription.offset = offset;
	m_VertexInputAttributeDescription.push_back(positionAttributeDescription);
}

std::vector<VkVertexInputBindingDescription> CVulkanVertexDescription::getBindingDescription()
{
	return this->m_VertexInputBindingDescription;
}

std::vector<VkVertexInputAttributeDescription> CVulkanVertexDescription::getAttributeDescription()
{
	return this->m_VertexInputAttributeDescription;
}
