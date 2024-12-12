#include "engine.pch.h"
#include "qp_render_camera.h"

#if defined( QP_VULKAN )

#include "qp/engine/resources/qp_resource_registry.h"
#include "qp_vulkan.h"
#include "qp_buffer_structs.h"
#include "qp_vertex_helper.h"
#include "qp/common/filesystem/qp_file.h"
#include "qp/engine/debug/qp_log.h"
#include "qp/common/containers/qp_array.h"
#include "qp/common/containers/qp_list.h"
#include "qp/common/containers/qp_set.h"
#include "qp/common/math/qp_quat.h"
#include "qp/common/time/qp_clock.h"
#include "qp/engine/resources/image/qp_image.h"
#include "qp/engine/resources/loaders/qp_tga_loader.h"
#include "qp/engine/window/qp_input_codes.h"
#include "qp/engine/window/qp_keyboard.h"
#include "qp/engine/window/qp_window.h"
#include <stdexcept>

//include vulkan last since it includes platform headers.
#include "vulkan/vulkan.h"

namespace qp {
#if defined( QP_DEV )
	const bool enableValidationLayers = true;
#else
	const bool enableValidationLayers = false;
#endif
	
	const int MAX_FRAMES_IN_FLIGHT = 2;
	
	static List< const char * > deviceExtensions {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};
	
	Array< vertex_t, 4 > meshVertices {
		vertex_t{.pos{-50.0f, -50.0f}, .color{1.0f, 0.0f, 0.0f}, .texCoord{1.0f, 1.0f}}, // top right
		vertex_t{.pos{50.0f, -50.0f}, .color{0.0f, 1.0f, 0.0f}, .texCoord{0.0f, 1.0f}}, // top left
		vertex_t{.pos{50.0f, 50.0f}, .color{0.0f, 0.0f, 1.0f}, .texCoord{0.0f, 0.0f}}, // bottom left
		vertex_t{.pos{-50.0f, 50.0f}, .color{1.0f, 1.0f, 1.0f}, .texCoord{1.0f, 0.0f}} // bottom right
	};
	
	Array< uint16, 6 > meshIndices {
		0, 1, 2, 2, 3, 0
	};
	
	Vulkan::Vulkan() { 
	}
	
	Vulkan::~Vulkan() {
		Cleanup();
	}
	
	void Vulkan::Init( const Window * window ) {
		m_window = window;
		CreateInstance();
		SetupDebugMessenger();
		CreateSurface();
		PickPhysicalDevice();
		CreateLogicalDevice();
		CreateSwapchain();
		CreateImageViews();
		CreateRenderPass();
		CreateDescriptorSetLayout();
		CreateGraphicsPipeline();
		CreateFrameBuffers();
		CreateCommandPool();
		CreateTextureImage();
		CreateTextureImageView();
		CreateTextureSampler();
		CreateVertexBuffer();
		CreateIndexBuffer();
		CreateUniformBuffers();
		CreateDescriptorPool();
		CreateDescriptorSets();
		CreateCommandBuffers();
		CreateSyncObjects();
	}
	
	void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks * allocator );
	
	void Vulkan::Cleanup() {
		vkDeviceWaitIdle( m_device );
	
		CleanupSyncObjects();
	
		vkDestroyBuffer( m_device, m_indexBuffer, NULL );
		vkFreeMemory( m_device, m_indexBufferMemory, NULL );
	
		vkDestroyBuffer( m_device, m_vertexBuffer, NULL );
		vkFreeMemory( m_device, m_vertexBufferMemory, NULL );
	
		vkDestroyCommandPool( m_device, m_commandPool, NULL );
	
		vkDestroyPipeline( m_device, m_graphicsPipeline, NULL );
		vkDestroyPipelineLayout( m_device, m_pipelineLayout, NULL );
		vkDestroyRenderPass( m_device, m_renderPass, NULL );
	
		for ( int index = 0; index < MAX_FRAMES_IN_FLIGHT; index++ ) {
			vkDestroyBuffer( m_device, m_uniformBuffers[ index ], NULL );
			vkFreeMemory( m_device, m_uniformBuffersMemory[ index ], NULL );
		}
	
		vkDestroyDescriptorPool( m_device, m_descriptorPool, NULL );
		vkDestroyDescriptorSetLayout( m_device, m_descriptorSetLayout, NULL );
	
		CleanupSwapchain();
	
		vkDestroySampler( m_device, m_textureSampler, NULL );
		vkDestroyImageView( m_device, m_textureImageView, NULL );
	
		vkDestroyImage( m_device, m_textureImage, NULL );
		vkFreeMemory( m_device, m_textureImageMemory, NULL );
	
		vkDestroyDevice( m_device, NULL );
		vkDestroySurfaceKHR( m_instance, m_surface, NULL );
	
		if constexpr ( enableValidationLayers ) {
			DestroyDebugUtilsMessengerEXT( m_instance, m_debugMessenger, NULL );
		}
		vkDestroyInstance( m_instance, NULL );
	}
	
	void InitializeDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT & createInfo );
	
	void Vulkan::CreateInstance() {
		VkApplicationInfo appInfo {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pNext = NULL;
		appInfo.pApplicationName = "qpVulkanApp";
		appInfo.pEngineName = "qp";
		appInfo.apiVersion = VK_API_VERSION_1_3;
	
		VkInstanceCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pNext = NULL;
		createInfo.flags = 0;
		createInfo.pApplicationInfo = &appInfo;
	
		List< const char * > enabledExtensions {
			VK_KHR_SURFACE_EXTENSION_NAME,
			VK_EXT_DEBUG_REPORT_EXTENSION_NAME
		};
	
#if defined( QP_PLATFORM_WINDOWS )
		enabledExtensions.Push( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );
#else
	#error( "Unsupported platform." );
#endif
	
		Array< const char *, 1 > validationLayers {
			"VK_LAYER_KHRONOS_validation"
		};
	
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
		if constexpr ( enableValidationLayers ) {
			if ( !CheckValidationLayerSupport( validationLayers ) ) {
				ThrowOnError( "Validation layers were requested but are not supported." );
			}
	
			InitializeDebugMessengerCreateInfo( debugCreateInfo );
	
			createInfo.enabledLayerCount = validationLayers.Length();
			createInfo.ppEnabledLayerNames = validationLayers.Data();
			createInfo.pNext = &debugCreateInfo;
	
			enabledExtensions.Push( VK_EXT_DEBUG_UTILS_EXTENSION_NAME );
	
		} else {
			createInfo.enabledLayerCount = 0;
		}
	
		createInfo.enabledExtensionCount = VerifyStaticCast< uint32 >( enabledExtensions.Length() );
		createInfo.ppEnabledExtensionNames = enabledExtensions.Data();
	
		VkResult result = vkCreateInstance( &createInfo, NULL, &m_instance );
	
		if ( result == VK_ERROR_INCOMPATIBLE_DRIVER ) {
			ThrowOnError( "Driver is incompatible with Vulkan. vkCreateInstance failed." );
		} else if ( result != VK_SUCCESS ) {
			ThrowOnError( "vkCreateInstance failed." );
		}
	
		debug::Trace( "Successfully initialized Vulkan." );
	}
	
	static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessageCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,		VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT * callbackData, void * userData ) {
	
		switch ( messageSeverity ) {
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
			{
				debug::Trace( "%s", callbackData->pMessage );
				break;
			}
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
			{
				debug::Info( "%s", callbackData->pMessage );
				break;
			}
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
			{
				debug::Warning( "%s", callbackData->pMessage );
				break;
			}
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
			{
				debug::Error( "%s", callbackData->pMessage );
				break;
			}
			case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
			{
				break;
			}
		}
	
		return VK_FALSE;
	}
	
	VkResult CreateDebugUtilsMessengerEXT( VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT * createInfo, const VkAllocationCallbacks	* allocator, VkDebugUtilsMessengerEXT * debugMessenger ) {
		PFN_vkCreateDebugUtilsMessengerEXT func = reinterpret_cast< PFN_vkCreateDebugUtilsMessengerEXT >( vkGetInstanceProcAddr( instance,	"vkCreateDebugUtilsMessengerEXT" ) );
		if ( func != NULL ) {
			return func( instance, createInfo, allocator, debugMessenger );
		}
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
	
	void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks * allocator ) {
		PFN_vkDestroyDebugUtilsMessengerEXT func = reinterpret_cast< PFN_vkDestroyDebugUtilsMessengerEXT >( vkGetInstanceProcAddr( instance,	"vkDestroyDebugUtilsMessengerEXT" ) );
		if ( func != NULL ) {
			func( instance, debugMessenger, allocator );
		}
	}
	
	void InitializeDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT & createInfo ) {
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT /*| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT*/ |	VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |		VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = DebugMessageCallback;
		createInfo.pUserData = NULL;
	}
	
	void Vulkan::SetupDebugMessenger() {
		if constexpr ( enableValidationLayers ) {
			VkDebugUtilsMessengerCreateInfoEXT createInfo {};
			InitializeDebugMessengerCreateInfo( createInfo );
	
			if ( CreateDebugUtilsMessengerEXT( m_instance, &createInfo, NULL, &m_debugMessenger ) != VK_SUCCESS ) {
				ThrowOnError( "Failed to set up debug messenger!" );
			}
		}
	}
	
	static bool CheckDeviceExtensionSupport( VkPhysicalDevice device ) {
		uint32 extensionCount;
		vkEnumerateDeviceExtensionProperties( device, NULL, &extensionCount, NULL );
	
		List< VkExtensionProperties > availableExtensions( extensionCount );
		vkEnumerateDeviceExtensionProperties( device, NULL, &extensionCount, availableExtensions.Data() );
	
		Set< String > requiredExtensions;
		for ( const char * extension : deviceExtensions ) {
			requiredExtensions.Emplace( extension );
		}
	
		for ( const VkExtensionProperties & extension : availableExtensions ) {
			requiredExtensions.Erase( extension.extensionName );
	
			if ( requiredExtensions.IsEmpty() ) {
				return true;
			}
		}
	
		return false;
	}
	
	bool Vulkan::IsDeviceSuitable( VkPhysicalDevice device ) {
		queueFamilyIndices_t indices = FindQueueFamilies( device );
		bool allIndices = HasAllQueueFamilyIndices( indices );
	
		bool extensionsSupported = CheckDeviceExtensionSupport( device );
	
		bool swapchainAdequate = false;
		if ( extensionsSupported ) {
			swapchainSupportDetails_t swapchainSupportDetails = QuerySwapchainSupport( device );
			swapchainAdequate = !swapchainSupportDetails.formats.IsEmpty() && !swapchainSupportDetails.presentModes.IsEmpty();
		}
	
		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures( device, &supportedFeatures );
	
		return allIndices && extensionsSupported && swapchainAdequate && supportedFeatures.samplerAnisotropy;
	}
	
	bool Vulkan::HasAllQueueFamilyIndices( const queueFamilyIndices_t & indices ) const {
		return indices.graphicsFamily.HasValue() && indices.presentFamily.HasValue();
	}
	
	void Vulkan::PickPhysicalDevice() {
		uint32 deviceCount = 0;
		vkEnumeratePhysicalDevices( m_instance, &deviceCount, NULL );
	
		if ( deviceCount == 0 ) {
			ThrowOnError( "Couldn't find any GPUs with Vulkan support!" );
		}
	
		List< VkPhysicalDevice > devices( deviceCount );
		vkEnumeratePhysicalDevices( m_instance, &deviceCount, devices.Data() );
	
		for ( VkPhysicalDevice device : devices ) {
			if ( IsDeviceSuitable( device ) ) {
				m_physicalDevice = device;
				break;
			}
		}
	
		if ( m_physicalDevice == NULL ) {
			ThrowOnError( "Couldn't find a suitable GPU!" );
		}
	}
	
	void Vulkan::CreateLogicalDevice() {
		queueFamilyIndices_t indices = FindQueueFamilies( m_physicalDevice );
	
		List< VkDeviceQueueCreateInfo > queueCreateInfos;
		Set< uint32 > uniqueQueueFamilies { indices.graphicsFamily.GetValue(), indices.presentFamily.GetValue() };
	
		float queuePriority = 1.0f;
	
		for ( uint32 queueFamily : uniqueQueueFamilies ) {
			VkDeviceQueueCreateInfo queueCreateInfo {};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.Push( queueCreateInfo );
		}
	
		VkPhysicalDeviceFeatures deviceFeatures {};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
	
		VkDeviceCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		createInfo.queueCreateInfoCount = VerifyStaticCast< uint32 >( queueCreateInfos.Length() );
		createInfo.pQueueCreateInfos = queueCreateInfos.Data();
		createInfo.pEnabledFeatures = &deviceFeatures;
		createInfo.enabledExtensionCount = VerifyStaticCast< uint32 >( deviceExtensions.Length() );
		createInfo.ppEnabledExtensionNames = deviceExtensions.Data();
	
		if ( vkCreateDevice( m_physicalDevice, &createInfo, NULL, &m_device ) ) {
			ThrowOnError( "Failed to create logical device!" );
		}
	
		if ( indices.graphicsFamily.HasValue() ) {
			vkGetDeviceQueue( m_device, indices.graphicsFamily.GetValue(), 0, &m_graphicsQueue );
		}
	
		if ( indices.presentFamily.HasValue() ) {
			vkGetDeviceQueue( m_device, indices.presentFamily.GetValue(), 0, &m_presentQueue );
		}
	}
	
	void Vulkan::CreateSurface() {
	
#if defined( QP_PLATFORM_WINDOWS )
		VkWin32SurfaceCreateInfoKHR createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
		createInfo.hwnd = static_cast< HWND >( m_window->GetHandle() );
		createInfo.hinstance = GetModuleHandle( NULL );
	
		if ( vkCreateWin32SurfaceKHR( m_instance, &createInfo, NULL, &m_surface ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to create win32 window surface!" );
		}
#endif
	
	}
	
	Vulkan::queueFamilyIndices_t Vulkan::FindQueueFamilies( VkPhysicalDevice device ) {
		queueFamilyIndices_t indices;
	
		uint32 queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, NULL );
	
		List< VkQueueFamilyProperties > queueFamilies( queueFamilyCount );
		vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, queueFamilies.Data() );
	
		for ( int index = 0; index < queueFamilies.Length(); index++ ) {
			const VkQueueFamilyProperties & queueFamily = queueFamilies[ index ];
			if ( queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT ) {
				indices.graphicsFamily = index;
			}
	
			uint32 presentSupport = 0;
			vkGetPhysicalDeviceSurfaceSupportKHR( device, index, m_surface, &presentSupport );
	
			if ( presentSupport != 0 ) {
				indices.presentFamily = index;
			}
	
			if ( HasAllQueueFamilyIndices( indices ) ) {
				break;
			}
		}
	
		return indices;
	}
	
	Vulkan::swapchainSupportDetails_t Vulkan::QuerySwapchainSupport( VkPhysicalDevice device ) {
		swapchainSupportDetails_t details;
	
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR( device, m_surface, &details.capabilities );
	
		uint32 formatCount;
		vkGetPhysicalDeviceSurfaceFormatsKHR( device, m_surface, &formatCount, NULL );
	
		if ( formatCount != 0 ) {
			details.formats.Resize( formatCount );
			vkGetPhysicalDeviceSurfaceFormatsKHR( device, m_surface, &formatCount, details.formats.Data() );
		}
	
		uint32 presentModeCount;
		vkGetPhysicalDeviceSurfacePresentModesKHR( device, m_surface, &presentModeCount, NULL );
	
		if ( presentModeCount != 0 ) {
			details.presentModes.Resize( presentModeCount );
			vkGetPhysicalDeviceSurfacePresentModesKHR( device, m_surface, &presentModeCount, details.presentModes.Data() );
		}
	
		return details;
	}
	
	VkSurfaceFormatKHR Vulkan::ChooseSwapchainSurfaceFormat( const List< VkSurfaceFormatKHR > & availableFormats ) {
		for ( const VkSurfaceFormatKHR & availableFormat : availableFormats ) {
			if ( availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ) {
				return availableFormat;
			}
		}
	
		return availableFormats[ 0 ];
	}
	
	VkPresentModeKHR Vulkan::ChooseSwapchainPresentMode( const List< VkPresentModeKHR > & availablePresentModes ) {
		for ( const VkPresentModeKHR & availablePresentMode : availablePresentModes ) {
			if ( availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR ) {
				return availablePresentMode;
			}
		}
	
		return VK_PRESENT_MODE_FIFO_KHR;
	}
	
	VkExtent2D Vulkan::ChooseSwapchainExtent( const VkSurfaceCapabilitiesKHR & capabilities ) {
		if ( capabilities.currentExtent.width != UINT32_MAX ) {
			return capabilities.currentExtent;
		}
	
		const int width = m_window->GetClientWidth();
		const int height = m_window->GetClientHeight();
		
		VkExtent2D actualExtent {
			static_cast< uint32 >( width ),
			static_cast< uint32 >( height )
		};
	
		actualExtent.width = math::Clamp( actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
		actualExtent.height = math::Clamp( actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );
	
		return actualExtent;
	}
	
	void Vulkan::CreateSwapchain() {
		swapchainSupportDetails_t swapchainSupportDetails = QuerySwapchainSupport( m_physicalDevice );
	
		VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainSurfaceFormat( swapchainSupportDetails.formats );
		VkPresentModeKHR presentMode = ChooseSwapchainPresentMode( swapchainSupportDetails.presentModes );
		VkExtent2D extent = ChooseSwapchainExtent( swapchainSupportDetails.capabilities );
	
		uint32 imageCount = swapchainSupportDetails.capabilities.minImageCount + 1;
	
		if ( ( swapchainSupportDetails.capabilities.maxImageCount > 0 ) && ( imageCount > swapchainSupportDetails.capabilities.maxImageCount ) ) {
			imageCount = swapchainSupportDetails.capabilities.maxImageCount;
		}
	
		VkSwapchainCreateInfoKHR createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_surface;
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	
		queueFamilyIndices_t indices = FindQueueFamilies( m_physicalDevice );
		uint32 queueFamilyIndices[] { indices.graphicsFamily.HasValue(), indices.presentFamily.HasValue() };
	
		if ( indices.graphicsFamily != indices.presentFamily ) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		} else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0; 
			createInfo.pQueueFamilyIndices = NULL; 
		}
	
		createInfo.preTransform = swapchainSupportDetails.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;
	
		if ( vkCreateSwapchainKHR( m_device, &createInfo, NULL, &m_swapchain ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to create swap chain!" );
		}
	
		vkGetSwapchainImagesKHR( m_device, m_swapchain, &imageCount, NULL );
		m_swapchainImages.Resize( imageCount );
		vkGetSwapchainImagesKHR( m_device, m_swapchain, &imageCount, m_swapchainImages.Data() );
	
		m_swapchainImageFormat = surfaceFormat.format;
		m_swapchainExtent = extent;
	}
	
	void Vulkan::CreateImageViews() {
		m_swapchainImageViews.Resize( m_swapchainImages.Length() );
	
		for( int index = 0; index < m_swapchainImages.Length(); index++ ) {
			m_swapchainImageViews[ index ] = CreateImageView( m_swapchainImages[ index ], m_swapchainImageFormat );
		}
	}
	
	void Vulkan::CreateRenderPass() {
		VkAttachmentDescription colorAttachment {};
		colorAttachment.format = m_swapchainImageFormat;
		colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	
		VkAttachmentReference colorAttachmentRef {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	
		VkSubpassDescription subpass {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
	
		VkSubpassDependency dependency {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	
		VkRenderPassCreateInfo renderPassInfo {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &colorAttachment;
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;
	
		if ( vkCreateRenderPass( m_device, &renderPassInfo, NULL, &m_renderPass ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to create render pass!" );
		}
	}
	
	void Vulkan::CreateDescriptorSetLayout() {
		VkDescriptorSetLayoutBinding uboLayoutBinding {};
		uboLayoutBinding.binding = 0;
		uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		uboLayoutBinding.descriptorCount = 1;
		uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		uboLayoutBinding.pImmutableSamplers = NULL;
	
		VkDescriptorSetLayoutBinding samplerLayoutBinding {};
		samplerLayoutBinding.binding = 1;
		samplerLayoutBinding.descriptorCount = 1;
		samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		samplerLayoutBinding.pImmutableSamplers = NULL;
		samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	
		Array< VkDescriptorSetLayoutBinding, 2 > bindings { uboLayoutBinding, samplerLayoutBinding };
		VkDescriptorSetLayoutCreateInfo layoutInfo {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = 2;
		layoutInfo.pBindings = bindings.Data();
	
		if ( vkCreateDescriptorSetLayout( m_device, &layoutInfo, NULL, &m_descriptorSetLayout ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to create descriptor set layout!" );
		}
	}
	
	VkShaderModule Vulkan::CreateShaderModule( const List< byte > & shaderCode ) {
		VkShaderModuleCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = shaderCode.Length();
		createInfo.pCode = reinterpret_cast< const uint32 * >( shaderCode.Data() );
	
		VkShaderModule shaderModule;
		if ( vkCreateShaderModule( m_device, &createInfo, NULL, &shaderModule ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to create shader module!" );
		}
		return shaderModule;
	}
	
	void Vulkan::CreateGraphicsPipeline() {
		List< byte > vertShaderCode = ReadFile( "shaders/spir-v/shader.vert.spv" );
		List< byte > fragShaderCode = ReadFile( "shaders/spir-v/shader.frag.spv" );
	
		VkShaderModule vertShaderModule = CreateShaderModule( vertShaderCode );
		VkShaderModule fragShaderModule = CreateShaderModule( fragShaderCode );
	
		VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";
	
		VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";
	
		Array< VkPipelineShaderStageCreateInfo, 2 > shaderStages {
			vertShaderStageInfo,
			fragShaderStageInfo
		};
	
		auto bindingDescription = vertex_helper::GetBindingDescription();
		auto attributeDescriptions = vertex_helper::GetAttributeDescriptions();
	
		VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputInfo.vertexBindingDescriptionCount = 1;
		vertexInputInfo.pVertexBindingDescriptions = &bindingDescription; 
		vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.Length();
		vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.Data();
	
		VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		inputAssembly.primitiveRestartEnable = VK_FALSE;
	
		VkViewport viewport {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast< float >( m_swapchainExtent.width );
		viewport.height = static_cast< float >( m_swapchainExtent.height );
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
	
		VkRect2D scissor {};
		scissor.offset = { 0, 0 };
		scissor.extent = m_swapchainExtent;
	
		List< VkDynamicState > dynamicStates = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_SCISSOR
		};
	
		VkPipelineDynamicStateCreateInfo dynamicState {};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast< uint32 >( dynamicStates.Length() );
		dynamicState.pDynamicStates = dynamicStates.Data();
	
		VkPipelineViewportStateCreateInfo viewportState {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;
	
		VkPipelineRasterizationStateCreateInfo rasterizer {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;
	
		VkPipelineMultisampleStateCreateInfo multisampling {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampling.minSampleShading = 1.0f;
		multisampling.pSampleMask = NULL;
		multisampling.alphaToCoverageEnable = VK_FALSE; 
		multisampling.alphaToOneEnable = VK_FALSE;
	
		VkPipelineColorBlendAttachmentState colorBlendAttachment {};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |	VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;
		colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; 
		colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; 
		colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; 
		colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; 
		colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; 
		colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; 
	
		VkPipelineColorBlendStateCreateInfo colorBlending {};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[ 0 ] = 0.0f; 
		colorBlending.blendConstants[ 1 ] = 0.0f; 
		colorBlending.blendConstants[ 2 ] = 0.0f; 
		colorBlending.blendConstants[ 3 ] = 0.0f;
	
		VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 0; 
		pipelineLayoutInfo.pPushConstantRanges = NULL; 
	
		if ( vkCreatePipelineLayout( m_device, &pipelineLayoutInfo, NULL, &m_pipelineLayout ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to create pipeline layout!" );
		}
	
		VkGraphicsPipelineCreateInfo pipelineInfo {};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages.Data();
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = NULL; // Optional
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_pipelineLayout;
		pipelineInfo.renderPass = m_renderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; 
		pipelineInfo.basePipelineIndex = -1; 
	
		if ( vkCreateGraphicsPipelines( m_device, VK_NULL_HANDLE, 1, &pipelineInfo, NULL, &m_graphicsPipeline ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to create graphics pipeline!" );
		}
	
		vkDestroyShaderModule( m_device, fragShaderModule, NULL );
		vkDestroyShaderModule( m_device, vertShaderModule, NULL );
	}
	
	void Vulkan::CreateFrameBuffers() {
		m_swapchainFramebuffers.Resize( m_swapchainImageViews.Length() );
	
		for ( int i = 0; i < m_swapchainImageViews.Length(); i++ ) {
			Array< VkImageView, 1 > attachments {
				m_swapchainImageViews[ i ]
			};
	
			VkFramebufferCreateInfo framebufferInfo {};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_renderPass;
			framebufferInfo.attachmentCount = 1;
			framebufferInfo.pAttachments = attachments.Data();
			framebufferInfo.width = m_swapchainExtent.width;
			framebufferInfo.height = m_swapchainExtent.height;
			framebufferInfo.layers = 1;
	
			if ( vkCreateFramebuffer( m_device, &framebufferInfo, NULL, &m_swapchainFramebuffers[ i ] ) != VK_SUCCESS ) {
				ThrowOnError( "Failed to create framebuffer!" );
			}
		}
	}
	
	void Vulkan::CreateCommandPool() {
		queueFamilyIndices_t queueFamilyIndices = FindQueueFamilies( m_physicalDevice );
	
		VkCommandPoolCreateInfo poolInfo {};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.GetValue();
	
		if ( vkCreateCommandPool( m_device, &poolInfo, NULL, &m_commandPool ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to create command pool!" );
		}
	}
	
	void Vulkan::CreateCommandBuffers() {
		m_commandBuffers.Resize( MAX_FRAMES_IN_FLIGHT );
	
		VkCommandBufferAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = VerifyStaticCast< uint32 >( m_commandBuffers.Length() );
	
		if ( vkAllocateCommandBuffers( m_device, &allocInfo, m_commandBuffers.Data() ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to allocate command buffers!" );
		}
	}
	
	void Vulkan::CreateSyncObjects() {
		m_imageAvailableSemaphores.Resize( MAX_FRAMES_IN_FLIGHT );
		m_renderFinishedSemaphores.Resize( MAX_FRAMES_IN_FLIGHT );
		m_inFlightFences.Resize( MAX_FRAMES_IN_FLIGHT );
	
		VkSemaphoreCreateInfo semaphoreInfo {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	
		VkFenceCreateInfo fenceInfo {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
	
		for( int index = 0; index < MAX_FRAMES_IN_FLIGHT; index++ ) {
			if ( vkCreateSemaphore( m_device, &semaphoreInfo, NULL, &m_imageAvailableSemaphores[ index ]) != VK_SUCCESS ) {
				ThrowOnError( Format( "Failed to create image available semaphore for frame %d!", index ) );
			}
	
			if ( vkCreateSemaphore( m_device, &semaphoreInfo, NULL, &m_renderFinishedSemaphores[ index ] ) != VK_SUCCESS ) {
				ThrowOnError( Format( "Failed to create render finished semaphore for frame %d!", index ) );
			}
	
			if ( vkCreateFence( m_device, &fenceInfo, NULL, &m_inFlightFences[ index ] ) != VK_SUCCESS ) {
				ThrowOnError( Format( "Failed to create in flight fence for frame %d!", index ) );
			}
		}
	}
	
	void Vulkan::CleanupSwapchain() {
		for ( VkFramebuffer framebuffer : m_swapchainFramebuffers ) {
			vkDestroyFramebuffer( m_device, framebuffer, NULL );
		}
	
		for ( VkImageView imageView : m_swapchainImageViews ) {
			vkDestroyImageView( m_device, imageView, NULL );
		}
	
		vkDestroySwapchainKHR( m_device, m_swapchain, NULL );
	}
	
	void Vulkan::CleanupSyncObjects() {
		for ( int index = 0; index < MAX_FRAMES_IN_FLIGHT; index++ ) {
			vkDestroySemaphore( m_device, m_imageAvailableSemaphores[ index ], NULL );
			vkDestroySemaphore( m_device, m_renderFinishedSemaphores[ index ], NULL );
			vkDestroyFence( m_device, m_inFlightFences[ index ], NULL );
		}
	}
	
	void Vulkan::RecreateSwapchain() {
		vkDeviceWaitIdle( m_device );
	
		CleanupSyncObjects();
		CleanupSwapchain();
	
		CreateSwapchain();
		CreateImageViews();
		CreateFrameBuffers();
		CreateSyncObjects();
	}
	
	void Vulkan::RecordCommandBuffer( VkCommandBuffer commandBuffer, int imageIndex ) {
		VkCommandBufferBeginInfo beginInfo {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0; 
		beginInfo.pInheritanceInfo = NULL; 
	
		if ( vkBeginCommandBuffer( commandBuffer, &beginInfo ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to begin recording command buffer!" );
		}
	
		VkRenderPassBeginInfo renderPassInfo {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_renderPass;
		renderPassInfo.framebuffer = m_swapchainFramebuffers[ imageIndex ];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_swapchainExtent;
	
		VkClearValue clearColor { { { 0.0f, 0.0f, 0.0f, 1.0f } } };
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;
	
		vkCmdBeginRenderPass( commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE );
	
		vkCmdBindPipeline( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline );
		VkViewport viewport {};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = static_cast< float >( m_swapchainExtent.width );
		viewport.height = static_cast< float >( m_swapchainExtent.height );
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;
		vkCmdSetViewport( commandBuffer, 0, 1, &viewport );
	
		VkRect2D scissor {};
		scissor.offset = { 0, 0 };
		scissor.extent = m_swapchainExtent;
		vkCmdSetScissor( commandBuffer, 0, 1, &scissor );
	
		VkBuffer vertexBuffers[] = { m_vertexBuffer };
		VkDeviceSize offsets[] = { 0 };
		vkCmdBindVertexBuffers( commandBuffer, 0, 1, vertexBuffers, offsets );
	
		vkCmdBindIndexBuffer( commandBuffer, m_indexBuffer, 0, VK_INDEX_TYPE_UINT16 );
	
		vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSets[ m_currentFrame ], 0,	NULL );
	
		vkCmdDrawIndexed( commandBuffer, meshIndices.Length(), 1, 0, 0, 0 );
	
		vkCmdEndRenderPass( commandBuffer );
	
		if ( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to record command buffer!" );
		}
	}
	
	void Vulkan::CreateVertexBuffer() {
		VkDeviceSize bufferSize = sizeof( meshVertices[ 0 ] ) * meshVertices.Length();
	
		VkBuffer stagingBuffer = NULL;
		VkDeviceMemory stagingBufferMemory = NULL;
		CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,		stagingBuffer, stagingBufferMemory );
	
		void * data = NULL;
		vkMapMemory( m_device, stagingBufferMemory, 0, bufferSize, 0, &data );
		memcpy( data, meshVertices.Data(), bufferSize );
		vkUnmapMemory( m_device, stagingBufferMemory );
	
		CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,	m_vertexBuffer, m_vertexBufferMemory );
	
		CopyBuffer( stagingBuffer, m_vertexBuffer, bufferSize );
		vkDestroyBuffer( m_device, stagingBuffer, NULL );
		vkFreeMemory( m_device, stagingBufferMemory, NULL );
	}
	
	void Vulkan::CreateIndexBuffer() {
		VkDeviceSize bufferSize = sizeof( meshIndices[ 0 ] ) * meshIndices.Length();
	
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,		stagingBuffer, stagingBufferMemory );
	
		void * data;
		vkMapMemory( m_device, stagingBufferMemory, 0, bufferSize, 0, &data );
		memcpy( data, meshIndices.Data(), bufferSize );
		vkUnmapMemory( m_device, stagingBufferMemory );
	
		CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,		m_indexBuffer, m_indexBufferMemory );
	
		CopyBuffer( stagingBuffer, m_indexBuffer, bufferSize );
	
		vkDestroyBuffer( m_device, stagingBuffer, NULL );
		vkFreeMemory( m_device, stagingBufferMemory, NULL );
	}
	
	void Vulkan::CreateUniformBuffers() {
		VkDeviceSize bufferSize = sizeof( uniformBufferObject_t );
	
		m_uniformBuffers.Resize( MAX_FRAMES_IN_FLIGHT );
		m_uniformBuffersMemory.Resize( MAX_FRAMES_IN_FLIGHT );
		m_uniformBuffersMapped.Resize( MAX_FRAMES_IN_FLIGHT );
	
		for ( int index = 0; index < MAX_FRAMES_IN_FLIGHT; index++ ) {
			CreateBuffer( bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffers[ index ], m_uniformBuffersMemory[ index ] );
	
			vkMapMemory( m_device, m_uniformBuffersMemory[ index ], 0, bufferSize, 0, &m_uniformBuffersMapped[ index ] );
		}
	}
	
	void Vulkan::CreateDescriptorPool() {
		Array< VkDescriptorPoolSize, 2 > poolSizes {};
		poolSizes[ 0 ].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[ 0 ].descriptorCount = MAX_FRAMES_IN_FLIGHT;
		poolSizes[ 1 ].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[ 1 ].descriptorCount = MAX_FRAMES_IN_FLIGHT;
	
		VkDescriptorPoolCreateInfo poolInfo {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = poolSizes.Length();
		poolInfo.pPoolSizes = poolSizes.Data();
		poolInfo.maxSets = static_cast< uint32_t >( MAX_FRAMES_IN_FLIGHT );
	
		if ( vkCreateDescriptorPool( m_device, &poolInfo, NULL, &m_descriptorPool ) != VK_SUCCESS ) {
			ThrowOnError( "failed to create descriptor pool!" );
		}
	}
	
	void Vulkan::CreateDescriptorSets() {
		List< VkDescriptorSetLayout > layouts( MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout );
		VkDescriptorSetAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_descriptorPool;
		allocInfo.descriptorSetCount = static_cast< uint32_t >( MAX_FRAMES_IN_FLIGHT );
		allocInfo.pSetLayouts = layouts.Data();
	
		m_descriptorSets.Resize( MAX_FRAMES_IN_FLIGHT );
		if ( vkAllocateDescriptorSets( m_device, &allocInfo, m_descriptorSets.Data() ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to allocate descriptor sets!" );
		}
	
		for ( int index = 0; index < MAX_FRAMES_IN_FLIGHT; index++ ) {
			VkDescriptorBufferInfo bufferInfo {};
			bufferInfo.buffer = m_uniformBuffers[ index ];
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof( uniformBufferObject_t );
	
			VkDescriptorImageInfo imageInfo {};
			imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			imageInfo.imageView = m_textureImageView;
			imageInfo.sampler = m_textureSampler;
	
			Array< VkWriteDescriptorSet, 2 > descriptorWrites {};
			descriptorWrites[ 0 ].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[ 0 ].dstSet = m_descriptorSets[ index ];
			descriptorWrites[ 0 ].dstBinding = 0;
			descriptorWrites[ 0 ].dstArrayElement = 0;
			descriptorWrites[ 0 ].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[ 0 ].descriptorCount = 1;
			descriptorWrites[ 0 ].pBufferInfo = &bufferInfo;
	
			descriptorWrites[ 1 ].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[ 1 ].dstSet = m_descriptorSets[ index ];
			descriptorWrites[ 1 ].dstBinding = 1;
			descriptorWrites[ 1 ].dstArrayElement = 0;
			descriptorWrites[ 1 ].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			descriptorWrites[ 1 ].descriptorCount = 1;
			descriptorWrites[ 1 ].pImageInfo = &imageInfo;
	
			vkUpdateDescriptorSets( m_device, descriptorWrites.Length(), descriptorWrites.Data(), 0, NULL );
		}
	
	}
	
	void Vulkan::CreateBuffer( VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer& outBuffer,	VkDeviceMemory& outBufferMemory ) {
		VkBufferCreateInfo bufferInfo {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usageFlags;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	
		if ( vkCreateBuffer( m_device, &bufferInfo, NULL, &outBuffer ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to create buffer!" );
		}
	
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements( m_device, outBuffer, &memRequirements );
	
		VkMemoryAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType( memRequirements.memoryTypeBits, propertyFlags );
	
		if ( vkAllocateMemory( m_device, &allocInfo, NULL, &outBufferMemory ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to allocate vertex buffer memory!" );
		}
	
		vkBindBufferMemory( m_device, outBuffer, outBufferMemory, 0 );
	}

	void Vulkan::CreateImage( uint32 width, uint32 height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags	properties, VkImage& image, VkDeviceMemory& imageMemory ) {
		VkImageCreateInfo imageInfo {};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	
		if ( vkCreateImage( m_device, &imageInfo, nullptr, &image ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to create image!" );
		}
	
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements( m_device, image, &memRequirements );
	
		VkMemoryAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = FindMemoryType( memRequirements.memoryTypeBits, properties );
	
		if ( vkAllocateMemory( m_device, &allocInfo, nullptr, &imageMemory ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to allocate image memory!" );
		}
	
		vkBindImageMemory( m_device, image, imageMemory, 0 );
	}
	
	VkImageView Vulkan::CreateImageView( VkImage image, VkFormat format ) {
		VkImageViewCreateInfo viewInfo {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;
	
		VkImageView imageView;
		if ( vkCreateImageView( m_device, &viewInfo, NULL, &imageView ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to create texture image view!" );
		}
	
		return imageView;
	}
	
	void Vulkan::CreateTextureImage() {
		ResourceRegistry registry;
		FilePath imagePath = "user/kat.tga";
		const Image * katImage = static_cast< const Image * >( registry.LoadResource( imagePath, returnDefault_t::RETURN_NULL ) );
		if ( registry.HasResourceError() ) {
			debug::Error( "Failed to load resource \"%s\" with error: %s", imagePath.c_str(), registry.GetLastResourceError().c_str() );
			ThrowOnError( "Failed to create image." );
		}
	
		//qpFile binFile;
		//if ( binFile.Open( "generated/user/kat_mip1.qpimage", fileAccessMode_t::QP_FILE_READ_WRITE ) ) {
		//	qpBinaryWriteSerializer writeSerializer;
		//	registry.SerializeResource( writeSerializer, katImage );
		//	binFile.Write( writeSerializer.GetBuffer(), writeSerializer.GetOffset() );
		//	binFile.Close();
		//}
	
		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		CreateBuffer( katImage->GetSize(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |	VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );
		void * data;
		vkMapMemory( m_device, stagingBufferMemory, 0, katImage->GetSize(), 0, &data );
		memcpy( data, katImage->GetData(), katImage->GetSize() );
		vkUnmapMemory( m_device, stagingBufferMemory );
	
		CreateImage( static_cast< uint32 >( katImage->GetWidth() ), static_cast< uint32 >( katImage->GetHeight() ), VK_FORMAT_R8G8B8A8_SRGB,	VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,	   m_textureImage, m_textureImageMemory );
	
		TransitionImageLayout( m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL );
		CopyBufferToImage( stagingBuffer, m_textureImage, static_cast< uint32 >( katImage->GetWidth() ), static_cast< uint32 >( katImage->GetHeight () ) );
		TransitionImageLayout( m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL );
	
		vkDestroyBuffer( m_device, stagingBuffer, NULL );
		vkFreeMemory( m_device, stagingBufferMemory, NULL );
	}

	void Vulkan::CreateTextureImageView() {
		m_textureImageView = CreateImageView( m_textureImage, VK_FORMAT_R8G8B8A8_SRGB );
	}
	
	void Vulkan::CreateTextureSampler() {
		VkSamplerCreateInfo samplerInfo {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.anisotropyEnable = VK_TRUE;
	
		VkPhysicalDeviceProperties properties {};
		vkGetPhysicalDeviceProperties( m_physicalDevice, &properties );
		samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
	
		samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		samplerInfo.unnormalizedCoordinates = VK_FALSE;
		samplerInfo.compareEnable = VK_FALSE;
		samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
		samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		samplerInfo.mipLodBias = 0.0f;
		samplerInfo.minLod = 0.0f;
		samplerInfo.maxLod = 0.0f;
	
		if ( vkCreateSampler( m_device, &samplerInfo, NULL, &m_textureSampler ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to create texture sampler!" );
		}
	}
	
	VkCommandBuffer Vulkan::BeginSingleTimeCommands() {
		VkCommandBufferAllocateInfo allocInfo {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = m_commandPool;
		allocInfo.commandBufferCount = 1;
	
		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers( m_device, &allocInfo, &commandBuffer );
	
		VkCommandBufferBeginInfo beginInfo {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
	
		vkBeginCommandBuffer( commandBuffer, &beginInfo );
		return commandBuffer;
	}
	
	void Vulkan::EndSingleTimeCommands( VkCommandBuffer commandBuffer ) {
		vkEndCommandBuffer( commandBuffer );
	
		VkSubmitInfo submitInfo {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
	
		vkQueueSubmit( m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE );
		vkQueueWaitIdle( m_graphicsQueue );
	
		vkFreeCommandBuffers( m_device, m_commandPool, 1, &commandBuffer );
	}
	
	void Vulkan::TransitionImageLayout( VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout ) {
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();
	
		VkImageMemoryBarrier barrier {};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
	
		VkPipelineStageFlags sourceStage = 0;
		VkPipelineStageFlags destinationStage = 0;
	
		if ( oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL ) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
	
			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		} else if ( oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL ) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
	
			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		} else {
			ThrowOnError( "Unsupported layout transition!" );
		}
	
		vkCmdPipelineBarrier( commandBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &barrier );
	
		EndSingleTimeCommands( commandBuffer );
	}
	
	void Vulkan::CopyBufferToImage( VkBuffer buffer, VkImage image, uint32 width, uint32 height ) {
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();
	
		VkBufferImageCopy region {};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
	
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
	
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};
	
		vkCmdCopyBufferToImage( commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region );
	
		EndSingleTimeCommands( commandBuffer );
	}
	
	void Vulkan::CopyBuffer( VkBuffer sourceBuffer, VkBuffer destinationBuffer, VkDeviceSize size ) {
		VkCommandBuffer commandBuffer = BeginSingleTimeCommands();
	
		VkBufferCopy copyRegion {};
		copyRegion.srcOffset = 0;
		copyRegion.dstOffset = 0;
		copyRegion.size = size;
		vkCmdCopyBuffer( commandBuffer, sourceBuffer, destinationBuffer, 1, &copyRegion );
	
		EndSingleTimeCommands( commandBuffer );
	}
	
	uint32 Vulkan::FindMemoryType( uint32 typeFilter, VkMemoryPropertyFlags properties ) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties( m_physicalDevice, &memProperties );
	
		for ( uint32 i = 0; i < memProperties.memoryTypeCount; i++ ) {
			if ( typeFilter & QP_BIT( i ) && ( ( memProperties.memoryTypes[ i ].propertyFlags & properties ) == properties ) ) {
				return i;
			}
		}
	
		ThrowOnError( "Failed to find suitable memory type!" );
	
#if defined( QP_DEBUG )
		return UINT32_MAX;
#endif
	}
	
	void UpdateUniformBuffer( void * mappedUBO, const renderCamera_t & renderCamera ) {
		uniformBufferObject_t ubo {};
		ubo.model = ( CreateRotationY( 180.0f ) * CreateTranslation( Vec3( 0.0f, 0.0f, 200.0f ) ) ).Transposed();
		ubo.view = renderCamera.view;
		ubo.projection = renderCamera.projection;
	
		Copy( static_cast< uniformBufferObject_t * >( mappedUBO ), 1, &ubo, 1 );
	}
	
	void Vulkan::DrawFrame( const renderCamera_t & renderCamera ) {
		const int width = m_window->GetClientWidth();
		const int height = m_window->GetClientHeight();
	
		if ( width == 0 || height == 0 ) {
			return;
		}
	
		vkWaitForFences( m_device, 1, &m_inFlightFences[ m_currentFrame ], VK_TRUE, UINT64_MAX );
	
		uint32 imageIndex;
		VkResult result = vkAcquireNextImageKHR( m_device, m_swapchain, UINT64_MAX, m_imageAvailableSemaphores[ m_currentFrame ], VK_NULL_HANDLE,	&imageIndex );
	
		if( ( result == VK_ERROR_OUT_OF_DATE_KHR ) || m_frameBufferResized ) {
			m_frameBufferResized = false;
			RecreateSwapchain();
			return;
		}
	
		if( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR ) {
			ThrowOnError( Format( "vkAcquireNextImageKHR failed with error code: %d", result ) );
		}
	
		vkResetFences( m_device, 1, &m_inFlightFences[ m_currentFrame ] );
	
		vkResetCommandBuffer( m_commandBuffers[ m_currentFrame ], 0 );
	
		RecordCommandBuffer( m_commandBuffers[ m_currentFrame ], imageIndex );
	
		UpdateUniformBuffer( m_uniformBuffersMapped[ m_currentFrame ], renderCamera );
	
		VkSubmitInfo submitInfo {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	
		Array< VkSemaphore, 1 > waitSemaphores {
			m_imageAvailableSemaphores[ m_currentFrame ]
		};
	
		Array< VkPipelineStageFlags, 1 > waitStages {
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
		};
		submitInfo.waitSemaphoreCount = waitSemaphores.Length();
		submitInfo.pWaitSemaphores = waitSemaphores.Data();
		submitInfo.pWaitDstStageMask = waitStages.Data();
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_commandBuffers[ m_currentFrame ];
	
		Array< VkSemaphore, 1 > signalSemaphores {
			m_renderFinishedSemaphores[ m_currentFrame ]
		};
		submitInfo.signalSemaphoreCount = signalSemaphores.Length();
		submitInfo.pSignalSemaphores = signalSemaphores.Data();
	
		if ( vkQueueSubmit( m_graphicsQueue, 1, &submitInfo, m_inFlightFences[ m_currentFrame ] ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to submit draw command buffer!" );
		}
	
		VkPresentInfoKHR presentInfo {};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	
		presentInfo.waitSemaphoreCount = signalSemaphores.Length();
		presentInfo.pWaitSemaphores = signalSemaphores.Data();
	
		VkSwapchainKHR swapChains[] = { m_swapchain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = NULL;
	
		result = vkQueuePresentKHR( m_presentQueue, &presentInfo );
	
		m_currentFrame = ( m_currentFrame + 1 ) % MAX_FRAMES_IN_FLIGHT;
	}
	
	bool Vulkan::CheckValidationLayerSupport( const ArrayView< const char * > & layersView ) {
		uint32 count;
		vkEnumerateInstanceLayerProperties( &count, NULL );
	
		List< VkLayerProperties > availableLayers( count );
		vkEnumerateInstanceLayerProperties( &count, availableLayers.Data() );
	
		for ( const char * layerName : layersView ) {
			bool found = false;
	
			for ( const VkLayerProperties & layerProperties : availableLayers ) {
				if ( StrEquals( layerName, layerProperties.layerName ) ) {
					found = true;
					break;
				}
			}
	
			if ( !found ) {
				return false;
			}
		}
	
		return true;
	}
	
	void Vulkan::ThrowOnError( const String & msg ) {
#if defined( QP_PLATFORM_WINDOWS )
		MessageBox( NULL, UTF8ToWide( msg ).c_str(), L"qp", MB_ICONERROR );
#else
		qpDebug::Error( "%s", msg.c_str() );
#endif
		throw std::runtime_error( msg.c_str() );
	}
}
#endif