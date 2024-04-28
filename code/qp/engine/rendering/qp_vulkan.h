#pragma once

#if defined( QP_VULKAN )
#include "qp/common/utilities/qp_optional.h"
#include "qp/common/containers/qp_array_view.h"
#include "qp/common/containers/qp_list.h"
#include "qp/common/string/qp_string.h"
#include <cstddef>
#include <vulkan/vulkan_core.h>

class qpWindow;

class qpVulkan {
public:
	qpVulkan();
	~qpVulkan();
	void Init( void * windowHandle, const qpWindow * window );
	void Cleanup();
	void DrawFrame();
	void RequestFramebufferResize();
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
	void CreateSurface( void * windowHandle );
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
	void CreateDescriptorSetLayout();
	void CreateGraphicsPipeline();
	void CreateFrameBuffers();
	void CreateCommandPool();
	void CreateCommandBuffers();
	void CreateSyncObjects();
	void CleanupSwapchain();
	void CleanupSyncObjects();
	void RecreateSwapchain();
	void RecordCommandBuffer( VkCommandBuffer commandBuffer, int imageIndex );
	void CreateVertexBuffer();
	void CreateIndexBuffer();
	void CreateUniformBuffers();
	void CreateDescriptorPool();
	void CreateDescriptorSets();
	void CreateBuffer( VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer & outBuffer, VkDeviceMemory & outBufferMemory );
	void CreateImage( uint32 width, uint32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage & image, VkDeviceMemory & imageMemory );
	VkImageView CreateImageView( VkImage image, VkFormat format );
	void CreateTextureImage();
	void CreateTextureImageView();
	void CreateTextureSampler();
	[[ nodiscard ]] VkCommandBuffer BeginSingleTimeCommands();
	void EndSingleTimeCommands( VkCommandBuffer commandBuffer );
	void TransitionImageLayout( VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout );
	void CopyBufferToImage( VkBuffer buffer, VkImage image, uint32 width, uint32 height );
	void CopyBuffer( VkBuffer sourceBuffer, VkBuffer destinationBuffer, VkDeviceSize size );
	uint32 FindMemoryType( uint32 typeFilter, VkMemoryPropertyFlags properties );
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
	VkDescriptorSetLayout m_descriptorSetLayout = NULL;
	VkDescriptorPool m_descriptorPool = NULL;
	qpList< VkDescriptorSet > m_descriptorSets;
	VkPipelineLayout m_pipelineLayout = NULL;
	VkPipeline m_graphicsPipeline = NULL;
	VkCommandPool m_commandPool = NULL;
	qpList< VkCommandBuffer > m_commandBuffers;
	qpList< VkImage > m_swapchainImages;
	qpList< VkImageView > m_swapchainImageViews;
	qpList< VkFramebuffer > m_swapchainFramebuffers;
	VkFormat m_swapchainImageFormat = VK_FORMAT_UNDEFINED;
	VkExtent2D m_swapchainExtent = { 0, 0 };
	VkQueue m_graphicsQueue = NULL;
	VkQueue m_presentQueue = NULL;
	qpList< VkSemaphore > m_imageAvailableSemaphores = NULL;
	qpList< VkSemaphore > m_renderFinishedSemaphores = NULL;
	qpList< VkFence > m_inFlightFences = NULL;
	VkBuffer m_vertexBuffer = NULL;
	VkDeviceMemory m_vertexBufferMemory = NULL;
	VkBuffer m_indexBuffer = NULL;
	VkDeviceMemory m_indexBufferMemory = NULL;
	qpList< VkBuffer > m_uniformBuffers;
	qpList< VkDeviceMemory > m_uniformBuffersMemory;
	qpList< void * > m_uniformBuffersMapped;
	VkImage m_textureImage = NULL;
	VkDeviceMemory m_textureImageMemory = NULL;
	VkImageView m_textureImageView = NULL;
	VkSampler m_textureSampler = NULL;
	int m_currentFrame = 0;
	bool m_framebufferResized = false;
	void * m_windowHandle = NULL;
};

#endif