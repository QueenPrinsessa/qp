#pragma once
#include "qp_vertex.h"
#include "qp/common/containers/qp_array.h"
#include <vulkan/vulkan_core.h>

class qpVertexHelper {
public:
	qpVertexHelper() = delete;
	~qpVertexHelper() = delete;
	static VkVertexInputBindingDescription GetBindingDescription();
	static qpArray< VkVertexInputAttributeDescription, 3 > GetAttributeDescriptions();
};

inline VkVertexInputBindingDescription qpVertexHelper::GetBindingDescription() {
	VkVertexInputBindingDescription bindingDescription {};
	bindingDescription.binding = 0;
	bindingDescription.stride = sizeof( vertex_t );
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	return bindingDescription;
}

inline qpArray< VkVertexInputAttributeDescription, 3 > qpVertexHelper::GetAttributeDescriptions() {
	qpArray< VkVertexInputAttributeDescription, 3 > attributeDescriptions;

	attributeDescriptions[ 0 ].binding = 0;
	attributeDescriptions[ 0 ].location = 0;
	attributeDescriptions[ 0 ].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[ 0 ].offset = offsetof( vertex_t, pos );

	attributeDescriptions[ 1 ].binding = 0;
	attributeDescriptions[ 1 ].location = 1;
	attributeDescriptions[ 1 ].format = VK_FORMAT_R32G32B32_SFLOAT;
	attributeDescriptions[ 1 ].offset = offsetof( vertex_t, color );

	attributeDescriptions[ 2 ].binding = 0;
	attributeDescriptions[ 2 ].location = 2;
	attributeDescriptions[ 2 ].format = VK_FORMAT_R32G32_SFLOAT;
	attributeDescriptions[ 2 ].offset = offsetof( vertex_t, texCoord );

	return attributeDescriptions;
}
