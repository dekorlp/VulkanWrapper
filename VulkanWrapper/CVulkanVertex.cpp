#include "VulkanVertex.h"

void CVulkanVertex::AddVertexInputBindingDescription(VkVertexInputBindingDescription bindingDescription)
{
	this->m_VertexInputBindingDescription.push_back(bindingDescription);
}

void CVulkanVertex::AddVertexInputAttributeDescription(VkVertexInputAttributeDescription attributeDescription)
{
	this->m_VertexInputAttributeDescription.push_back(attributeDescription);
}

std::vector<VkVertexInputBindingDescription> CVulkanVertex::getBindingDescription()
{
	return this->m_VertexInputBindingDescription;
}

std::vector<VkVertexInputAttributeDescription> CVulkanVertex::getAttributeDescription()
{
	return this->m_VertexInputAttributeDescription;
}