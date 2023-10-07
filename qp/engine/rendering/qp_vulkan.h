#pragma once
#include "qp/common/utilities/qp_optional.h"
#include "qp/tools/containers/qp_array_view.h"
#include "qp/tools/containers/qp_list.h"
#include "qp/tools/string/qp_string.h"
#include <cstddef>
#include <vulkan/vulkan.h>

class qpVulkan {
public:
	qpVulkan();
	~qpVulkan();
	void Init( void * windowHandle );
	void Cleanup();
private:
	struct queueFamilyIndices_t {
		Optional< uint32 > graphicsFamily;
		Optional< uint32 > presentFamily;
	};
	struct swapchainSupportDetails_t {
		VkSurfaceCapabilitiesKHR capabilities {};
		qpList< VkSurfaceFormatKHR > formats;
		qpList< VkPresentModeKHR > presentModes;
	};
	void CreateInstance();
	void SetupDebugMessenger();
	void PickPhysicalDevice();
	void CreateLogicalDevice();
	void CreateSurface( void * m_windowHandle );
	bool IsDeviceSuitable( VkPhysicalDevice device );
	bool HasAllQueueFamilyIndices( const queueFamilyIndices_t & indices ) const;
	queueFamilyIndices_t FindQueueFamilies( VkPhysicalDevice device );
	swapchainSupportDetails_t QuerySwapchainSupport( VkPhysicalDevice device );
	VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat( const qpList< VkSurfaceFormatKHR > & availableFormats );
	VkPresentModeKHR ChooseSwapchainPresentMode( const qpList< VkPresentModeKHR > & availablePresentModes );
	VkExtent2D ChooseSwapchainExtent( const VkSurfaceCapabilitiesKHR & capabilities );
	void CreateSwapchain();
	void CreateImageViews();
	void CreateRenderPass();
	void CreateGraphicsPipeline();
	VkShaderModule CreateShaderModule( const qpList< byte > & shaderCode );
	bool CheckValidationLayerSupport( const qpArrayView< const char * > & layersView );
	void ThrowOnError( const qpString & msg );
	VkInstance m_instance = NULL;
	VkPhysicalDevice m_physicalDevice = NULL;
	VkDevice m_device = NULL;
	VkSurfaceKHR m_surface = NULL;
	VkDebugUtilsMessengerEXT m_debugMessenger = NULL;
	VkSwapchainKHR m_swapchain = NULL;
	VkRenderPass m_renderPass = NULL;
	VkPipelineLayout m_pipelineLayout = NULL;
	VkPipeline m_graphicsPipeline = NULL;
	qpList< VkImage > m_swapchainImages;
	qpList< VkImageView > m_swapchainImageViews;
	VkFormat m_swapchainImageFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D m_swapchainExtent = { 0, 0 };
	VkQueue m_graphicsQueue = NULL;
	VkQueue m_presentQueue = NULL;
	

	void * m_windowHandle = NULL;
};
