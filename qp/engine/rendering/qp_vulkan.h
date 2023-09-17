#pragma once
#include "qp/tools/string/qp_string.h"
#include <cstddef>
#include <vulkan/vulkan.h>

class qpVulkan {
public:
	qpVulkan();
	~qpVulkan();
	void Init();
	void Cleanup();
private:
	void CreateInstance();
	void ThrowOnError( const qpString & msg );

	VkInstance m_instance = NULL;
};
