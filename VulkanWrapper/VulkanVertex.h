#pragma once
#include <vector>
#include <vulkan\vulkan.h>

class CVulkanVertex
{
	public:

		std::vector<VkVertexInputBindingDescription> getBindingDescription();
		std::vector<VkVertexInputAttributeDescription> getAttributeDescription();
	protected:
		void AddVertexInputBindingDescription(VkVertexInputBindingDescription bindingDescription);
		void AddVertexInputAttributeDescription(VkVertexInputAttributeDescription AttributeDescription);

		std::vector<VkVertexInputBindingDescription> m_VertexInputBindingDescription;
		std::vector <VkVertexInputAttributeDescription> m_VertexInputAttributeDescription;
};
