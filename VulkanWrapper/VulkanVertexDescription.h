#pragma once
#include <vector>
#include <vulkan\vulkan.h>

class CVulkanVertexDescription
{
	public:

		std::vector<VkVertexInputBindingDescription> getBindingDescription();
		std::vector<VkVertexInputAttributeDescription> getAttributeDescription();
	protected:
		void AddVertexInputBindingDescription(VkVertexInputBindingDescription bindingDescription);
		void AddVertexInputAttributeDescription(VkVertexInputAttributeDescription AttributeDescription);

		void AddVertexInputBindingDescription(size_t SizeOfVertexType, unsigned int binding);
		void AddVertexInputAttributeDescription(size_t offset, unsigned int binding, unsigned int location, VkFormat format);

		std::vector<VkVertexInputBindingDescription> m_VertexInputBindingDescription;
		std::vector <VkVertexInputAttributeDescription> m_VertexInputAttributeDescription;
};
