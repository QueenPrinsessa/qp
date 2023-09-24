#pragma once
#include "qp/tools/containers/qp_array_view.h"
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
	void SetupDebugMessenger();
	void PickPhysicalDevice();
	bool CheckValidationLayerSupport( const qpArrayView< const char * > & layersView );
	void ThrowOnError( const qpString & msg );

	VkInstance m_instance = NULL;
	VkPhysicalDevice m_physicalDevice = NULL;
	VkDebugUtilsMessengerEXT m_debugMessenger = NULL;
};
