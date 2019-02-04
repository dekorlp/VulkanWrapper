#pragma once
#include "VulkanVertex.h"
#include <glm/glm.hpp>

class CCustomVertex : public CVulkanVertex
{
	public:
		glm::vec2 pos;
		glm::vec3 color;

		CCustomVertex()
		{
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(CCustomVertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			m_VertexInputBindingDescription.push_back(bindingDescription);

			VkVertexInputAttributeDescription positionAttributeDescription;
			positionAttributeDescription.binding = 0;
			positionAttributeDescription.location = 0;
			positionAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
			positionAttributeDescription.offset = offsetof(CCustomVertex, pos);
			m_VertexInputAttributeDescription.push_back(positionAttributeDescription);

			VkVertexInputAttributeDescription colorAttributeDescription;
			colorAttributeDescription.binding = 0;
			colorAttributeDescription.location = 1;
			colorAttributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
			colorAttributeDescription.offset = offsetof(CCustomVertex, color);
			m_VertexInputAttributeDescription.push_back(colorAttributeDescription);
		}

		CCustomVertex(glm::vec2 pos, glm::vec3 color)
		{
			this->pos = pos;
			this->color = color;
		}
};