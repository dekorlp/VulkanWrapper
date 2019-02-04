#pragma once
//#include <glm/glm.hpp>
//#include <array>
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


/*
struct SVulkanVertex {
	glm::vec2 pos;
	glm::vec3 color;

	static VkVertexInputBindingDescription getBindingDescription() {
		VkVertexInputBindingDescription bindingDescription = {};
		bindingDescription.binding = 0;
		bindingDescription.stride = sizeof(SVulkanVertex);
		bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		return bindingDescription;
	}

	static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
		std::array<VkVertexInputAttributeDescription, 2> attributeDescriptions = {};
		attributeDescriptions[0].binding = 0;
		attributeDescriptions[0].location = 0;
		attributeDescriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
		attributeDescriptions[0].offset = offsetof(SVulkanVertex, pos);

		attributeDescriptions[1].binding = 0;
		attributeDescriptions[1].location = 1;
		attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		attributeDescriptions[1].offset = offsetof(SVulkanVertex, color);
		return attributeDescriptions;
	}
};*/

/*const std::vector<SVulkanVertex> vertices = {
	{ { -0.5f, -0.5f },{ 1.0f, 0.0f, 0.0f } },
	{ { 0.5f, -0.5f },{ 0.0f, 1.0f, 0.0f } },
	{ { 0.5f, 0.5f },{ 0.0f, 0.0f, 1.0f } },
	{ { -0.5f, 0.5f },{ 1.0f, 1.0f, 1.0f } }
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0
};*/