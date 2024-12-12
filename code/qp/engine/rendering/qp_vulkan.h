#pragma once

#if defined( QP_VULKAN )
#include "qp/common/utilities/qp_optional.h"
#include "qp/common/containers/qp_array_view.h"
#include "qp/common/containers/qp_list.h"
#include "qp/common/string/qp_string.h"
#include <vulkan/vulkan_core.h>
#include "qp_graphics_api.h"

namespace qp {
	class Window;

	class Vulkan final : public GraphicsAPI {
	public:
		Vulkan();
		virtual ~Vulkan() override;
		virtual void Init( const Window * window ) override;
		virtual void DrawFrame( const renderCamera_t & renderCamera ) override;
		virtual void Cleanup() override;

	private:
		struct queueFamilyIndices_t {
			Optional< uint32 > graphicsFamily;
			Optional< uint32 > presentFamily;
		};
		struct swapchainSupportDetails_t {
			VkSurfaceCapabilitiesKHR capabilities {};
			List< VkSurfaceFormatKHR > formats;
			List< VkPresentModeKHR > presentModes;
		};
		void CreateInstance();
		void SetupDebugMessenger();
		void PickPhysicalDevice();
		void CreateLogicalDevice();
		void CreateSurface();
		bool IsDeviceSuitable( VkPhysicalDevice device );
		bool HasAllQueueFamilyIndices( const queueFamilyIndices_t & indices ) const;
		queueFamilyIndices_t FindQueueFamilies( VkPhysicalDevice device );
		swapchainSupportDetails_t QuerySwapchainSupport( VkPhysicalDevice device );
		VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat( const List< VkSurfaceFormatKHR > & availableFormats );
		VkPresentModeKHR ChooseSwapchainPresentMode( const List< VkPresentModeKHR > & availablePresentModes );
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
		VkShaderModule CreateShaderModule( const List< byte > & shaderCode );
		bool CheckValidationLayerSupport( const ArrayView< const char * > & layersView );
		void ThrowOnError( const String & msg );
		VkInstance m_instance = NULL;
		VkPhysicalDevice m_physicalDevice = NULL;
		VkDevice m_device = NULL;
		VkSurfaceKHR m_surface = NULL;
		VkDebugUtilsMessengerEXT m_debugMessenger = NULL;
		VkSwapchainKHR m_swapchain = NULL;
		VkRenderPass m_renderPass = NULL;
		VkDescriptorSetLayout m_descriptorSetLayout = NULL;
		VkDescriptorPool m_descriptorPool = NULL;
		List< VkDescriptorSet > m_descriptorSets;
		VkPipelineLayout m_pipelineLayout = NULL;
		VkPipeline m_graphicsPipeline = NULL;
		VkCommandPool m_commandPool = NULL;
		List< VkCommandBuffer > m_commandBuffers;
		List< VkImage > m_swapchainImages;
		List< VkImageView > m_swapchainImageViews;
		List< VkFramebuffer > m_swapchainFramebuffers;
		VkFormat m_swapchainImageFormat = VK_FORMAT_UNDEFINED;
		VkExtent2D m_swapchainExtent = { 0, 0 };
		VkQueue m_graphicsQueue = NULL;
		VkQueue m_presentQueue = NULL;
		List< VkSemaphore > m_imageAvailableSemaphores = NULL;
		List< VkSemaphore > m_renderFinishedSemaphores = NULL;
		List< VkFence > m_inFlightFences = NULL;
		VkBuffer m_vertexBuffer = NULL;
		VkDeviceMemory m_vertexBufferMemory = NULL;
		VkBuffer m_indexBuffer = NULL;
		VkDeviceMemory m_indexBufferMemory = NULL;
		List< VkBuffer > m_uniformBuffers;
		List< VkDeviceMemory > m_uniformBuffersMemory;
		List< void * > m_uniformBuffersMapped;
		VkImage m_textureImage = NULL;
		VkDeviceMemory m_textureImageMemory = NULL;
		VkImageView m_textureImageView = NULL;
		VkSampler m_textureSampler = NULL;
		int m_currentFrame = 0;
		const Window * m_window = NULL;
	};
}
#endif