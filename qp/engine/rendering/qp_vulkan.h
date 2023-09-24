#pragma once
#include "qp/common/utilities/qp_optional.h"
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
	struct queueFamilyIndices_t {
		Optional< uint32 > graphicsFamily;
	};
	void CreateInstance();
	void SetupDebugMessenger();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	bool IsDeviceSuitable( VkPhysicalDevice device );
	bool HasAllQueueFamilyIndices( const queueFamilyIndices_t & indices ) const;
	queueFamilyIndices_t FindQueueFamilies( VkPhysicalDevice device );
	bool CheckValidationLayerSupport( const qpArrayView< const char * > & layersView );
	void ThrowOnError( const qpString & msg );
	VkInstance m_instance = NULL;
	VkPhysicalDevice m_physicalDevice = NULL;
	VkDevice m_device = NULL;
	VkQueue m_graphicsQueue = NULL;
	VkDebugUtilsMessengerEXT m_debugMessenger = NULL;
};
