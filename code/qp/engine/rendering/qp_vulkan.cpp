#include "engine.pch.h"
#include "qp_vulkan.h"
#include "qp_buffer_structs.h"
#include "qp_vertex_helper.h"
#include "qp/common/filesystem/qp_file.h"
#include "qp/engine/debug/qp_debug.h"
#include "qp/common/containers/qp_array.h"
#include "qp/common/containers/qp_list.h"
#include "qp/common/containers/qp_set.h"
#include "qp/common/math/qp_quat.h"
#include "qp/common/time/qp_clock.h"
#include "qp/engine/window/qp_keyboard.h"
#include "qp/engine/window/qp_window.h"
#include "vulkan/vulkan.h"
#include <stdexcept>
#include <iostream>
#include <qp/common/utilities/qp_optional.h>

#if defined( QP_DEV )
const bool enableValidationLayers = true;
#else
const bool enableValidationLayers = false;
#endif

const int MAX_FRAMES_IN_FLIGHT = 2;

static qpList< const char * > deviceExtensions {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

qpArray< vertex_t, 4 > meshVertices {
	vertex_t{.pos{-50.0f, -50.0f}, .color{1.0f, 0.0f, 0.0f}},
	vertex_t{.pos{50.0f, -50.0f}, .color{0.0f, 1.0f, 0.0f}},
	vertex_t{.pos{50.0f, 50.0f}, .color{0.0f, 0.0f, 1.0f}},
	vertex_t{.pos{-50.0f, 50.0f}, .color{1.0f, 1.0f, 1.0f}}
};

qpArray< uint16, 6 > meshIndices {
	0, 1, 2, 2, 3, 0
};

static void GetWindowFramebufferSize( void * windowHandle, int & width, int & height) {
#if defined( QP_PLATFORM_WINDOWS )
	RECT rect;
	GetClientRect( static_cast< HWND >( windowHandle ), &rect );

	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
#else
#error( "Unsupported platform." );
#endif
}

qpVulkan::qpVulkan() { }

qpVulkan::~qpVulkan() {
	Cleanup();
}
const qpWindow * windowForTesting = NULL;
void qpVulkan::Init( void * windowHandle, const qpWindow * window ) {
	m_windowHandle = windowHandle;
	windowForTesting = window;
	CreateInstance();
	SetupDebugMessenger();
	CreateSurface( windowHandle );
	PickPhysicalDevice();
	CreateLogicalDevice();
	CreateSwapchain();
	CreateImageViews();
	CreateRenderPass();
	CreateDescriptorSetLayout();
	CreateGraphicsPipeline();
	CreateFrameBuffers();
	CreateCommandPool();
	CreateVertexBuffer();
	CreateIndexBuffer();
	CreateUniformBuffers();
	CreateDescriptorPool();
	CreateDescriptorSets();
	CreateCommandBuffers();
	CreateSyncObjects();
}

void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks * allocator );

void qpVulkan::Cleanup() {
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

	vkDestroyDevice( m_device, NULL );
	vkDestroySurfaceKHR( m_instance, m_surface, NULL );

	if ( enableValidationLayers ) {
		DestroyDebugUtilsMessengerEXT( m_instance, m_debugMessenger, NULL );
	}

	vkDestroyInstance( m_instance, NULL );
}

void InitializeDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT & createInfo );

void qpVulkan::CreateInstance() {
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

	qpList< const char * > enabledExtensions {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME
	};

#if defined( QP_PLATFORM_WINDOWS )
	enabledExtensions.Push( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );
#else
#error( "Unsupported platform." );
#endif

	qpArray< const char *, 1 > validationLayers {
		"VK_LAYER_KHRONOS_validation"
	};

	VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
	if ( enableValidationLayers ) {
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

	createInfo.enabledExtensionCount = enabledExtensions.Length();
	createInfo.ppEnabledExtensionNames = enabledExtensions.Data();

	VkResult result = vkCreateInstance( &createInfo, NULL, &m_instance );

	if ( result == VK_ERROR_INCOMPATIBLE_DRIVER ) {
		ThrowOnError( "Driver is incompatible with Vulkan. vkCreateInstance failed." );
	} else if ( result != VK_SUCCESS ) {
		ThrowOnError( "vkCreateInstance failed." );
	}

	qpDebug::Log( "Successfully initialized Vulkan." );
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugMessageCallback( VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT * callbackData, void * userData ) {

	switch ( messageSeverity ) {
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
		{
			qpDebug::Log( "%s", callbackData->pMessage );
			break;
		}
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
		{
			qpDebug::Info( "%s", callbackData->pMessage );
			break;
		}
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
		{
			qpDebug::Warning( "%s", callbackData->pMessage );
			break;
		}
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
		{
			qpDebug::Error( "%s", callbackData->pMessage );
			break;
		}
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT:
		{
			break;
		}
	}

	return VK_FALSE;
}

VkResult CreateDebugUtilsMessengerEXT( VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT * createInfo, const VkAllocationCallbacks * allocator, VkDebugUtilsMessengerEXT * debugMessenger ) {
	PFN_vkCreateDebugUtilsMessengerEXT func = reinterpret_cast< PFN_vkCreateDebugUtilsMessengerEXT >( vkGetInstanceProcAddr( instance, "vkCreateDebugUtilsMessengerEXT" ) );
	if ( func != NULL ) {
		return func( instance, createInfo, allocator, debugMessenger );
	}
	return VK_ERROR_EXTENSION_NOT_PRESENT;
}

void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks * allocator ) {
	PFN_vkDestroyDebugUtilsMessengerEXT func = reinterpret_cast< PFN_vkDestroyDebugUtilsMessengerEXT >( vkGetInstanceProcAddr( instance, "vkDestroyDebugUtilsMessengerEXT" ) );
	if ( func != NULL ) {
		func( instance, debugMessenger, allocator );
	}
}

void InitializeDebugMessengerCreateInfo( VkDebugUtilsMessengerCreateInfoEXT & createInfo ) {
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
	createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT /*| VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT*/ | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
	createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
	createInfo.pfnUserCallback = DebugMessageCallback;
	createInfo.pUserData = NULL;
}

void qpVulkan::SetupDebugMessenger() {
	if constexpr ( !enableValidationLayers ) {
		return;
	}

	VkDebugUtilsMessengerCreateInfoEXT createInfo {};
	InitializeDebugMessengerCreateInfo( createInfo );

	if ( CreateDebugUtilsMessengerEXT( m_instance, &createInfo, NULL, &m_debugMessenger ) != VK_SUCCESS ) {
		ThrowOnError( "Failed to set up debug messenger!" );
	}
}

static bool CheckDeviceExtensionSupport( VkPhysicalDevice device ) {
	uint32 extensionCount;
	vkEnumerateDeviceExtensionProperties( device, NULL, &extensionCount, NULL );

	qpList< VkExtensionProperties > availableExtensions( extensionCount );
	vkEnumerateDeviceExtensionProperties( device, NULL, &extensionCount, availableExtensions.Data() );

	qpSet< qpString > requiredExtensions;
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

bool qpVulkan::IsDeviceSuitable( VkPhysicalDevice device ) {
	queueFamilyIndices_t indices = FindQueueFamilies( device );
	bool allIndices = HasAllQueueFamilyIndices( indices );

	bool extensionsSupported = CheckDeviceExtensionSupport( device );

	bool swapchainAdequate = false;
	if ( extensionsSupported ) {
		swapchainSupportDetails_t swapchainSupportDetails = QuerySwapchainSupport( device );
		swapchainAdequate = !swapchainSupportDetails.formats.IsEmpty() && !swapchainSupportDetails.presentModes.IsEmpty();
	}

	return allIndices && extensionsSupported && swapchainAdequate;
}

bool qpVulkan::HasAllQueueFamilyIndices( const queueFamilyIndices_t & indices ) const {
	return indices.graphicsFamily.HasValue() && indices.presentFamily.HasValue();
}

void qpVulkan::PickPhysicalDevice() {
	uint32 deviceCount = 0;
	vkEnumeratePhysicalDevices( m_instance, &deviceCount, NULL );

	if ( deviceCount == 0 ) {
		ThrowOnError( "Couldn't find any GPUs with Vulkan support!" );
	}

	qpList< VkPhysicalDevice > devices( deviceCount );
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

void qpVulkan::CreateLogicalDevice() {
	queueFamilyIndices_t indices = FindQueueFamilies( m_physicalDevice );

	qpList< VkDeviceQueueCreateInfo > queueCreateInfos;
	qpSet< uint32 > uniqueQueueFamilies { indices.graphicsFamily.GetValue(), indices.presentFamily.GetValue() };

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

	VkDeviceCreateInfo createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	createInfo.queueCreateInfoCount = queueCreateInfos.Length();
	createInfo.pQueueCreateInfos = queueCreateInfos.Data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = deviceExtensions.Length();
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

void qpVulkan::CreateSurface( void * windowHandle ) {

#if defined( QP_PLATFORM_WINDOWS )
	VkWin32SurfaceCreateInfoKHR createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = static_cast< HWND >( windowHandle );
	createInfo.hinstance = GetModuleHandle( NULL );

	if ( vkCreateWin32SurfaceKHR( m_instance, &createInfo, NULL, &m_surface ) != VK_SUCCESS ) {
		ThrowOnError( "Failed to create win32 window surface!" );
	}
#endif

}

qpVulkan::queueFamilyIndices_t qpVulkan::FindQueueFamilies( VkPhysicalDevice device ) {
	queueFamilyIndices_t indices;

	uint32 queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties( device, &queueFamilyCount, NULL );

	qpList< VkQueueFamilyProperties > queueFamilies( queueFamilyCount );
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

qpVulkan::swapchainSupportDetails_t qpVulkan::QuerySwapchainSupport( VkPhysicalDevice device ) {
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

VkSurfaceFormatKHR qpVulkan::ChooseSwapchainSurfaceFormat( const qpList< VkSurfaceFormatKHR > & availableFormats ) {
	for ( const VkSurfaceFormatKHR & availableFormat : availableFormats ) {
		if ( availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR ) {
			return availableFormat;
		}
	}

	return availableFormats[ 0 ];
}

VkPresentModeKHR qpVulkan::ChooseSwapchainPresentMode( const qpList< VkPresentModeKHR > & availablePresentModes ) {
	for ( const VkPresentModeKHR & availablePresentMode : availablePresentModes ) {
		if ( availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR ) {
			return availablePresentMode;
		}
	}

	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D qpVulkan::ChooseSwapchainExtent( const VkSurfaceCapabilitiesKHR & capabilities ) {
	if ( capabilities.currentExtent.width != UINT32_MAX ) {
		return capabilities.currentExtent;
	}

	int width;
	int height;

	GetWindowFramebufferSize( m_windowHandle, width, height );

	VkExtent2D actualExtent {
		static_cast< uint32 >( width ),
		static_cast< uint32 >( height )
	};

	actualExtent.width = qpMath::Clamp( actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width );
	actualExtent.height = qpMath::Clamp( actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height );

	return actualExtent;
}

void qpVulkan::CreateSwapchain() {
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

void qpVulkan::CreateImageViews() {
	m_swapchainImageViews.Resize( m_swapchainImages.Length() );

	for( int index = 0; index < m_swapchainImages.Length(); index++ ) {
		VkImageViewCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		createInfo.image = m_swapchainImages[ index ];
		createInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		createInfo.format = m_swapchainImageFormat;
		createInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		createInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		createInfo.subresourceRange.baseMipLevel = 0;
		createInfo.subresourceRange.levelCount = 1;
		createInfo.subresourceRange.baseArrayLayer = 0;
		createInfo.subresourceRange.layerCount = 1;

		if ( vkCreateImageView( m_device, &createInfo, NULL, &m_swapchainImageViews[ index ] ) != VK_SUCCESS ) {
			ThrowOnError( "Failed to create image views!" );
		}
	}
}

void qpVulkan::CreateRenderPass() {
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

void qpVulkan::CreateDescriptorSetLayout() {
	VkDescriptorSetLayoutBinding uboLayoutBinding {};
	uboLayoutBinding.binding = 0;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	uboLayoutBinding.pImmutableSamplers = NULL;

	VkDescriptorSetLayoutCreateInfo layoutInfo {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;

	if ( vkCreateDescriptorSetLayout( m_device, &layoutInfo, NULL, &m_descriptorSetLayout ) != VK_SUCCESS ) {
		ThrowOnError( "Failed to create descriptor set layout!" );
	}
}

VkShaderModule qpVulkan::CreateShaderModule( const qpList< byte > & shaderCode ) {
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

void qpVulkan::CreateGraphicsPipeline() {
	qpList< byte > vertShaderCode = qpReadFile( "shaders/spir-v/shader.vert.spv" );
	qpList< byte > fragShaderCode = qpReadFile( "shaders/spir-v/shader.frag.spv" );

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

	qpArray< VkPipelineShaderStageCreateInfo, 2 > shaderStages {
		vertShaderStageInfo,
		fragShaderStageInfo
	};

	auto bindingDescription = qpVertexHelper::GetBindingDescription();
	auto attributeDescriptions = qpVertexHelper::GetAttributeDescriptions();

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

	qpList< VkDynamicState > dynamicStates = {
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
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
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

void qpVulkan::CreateFrameBuffers() {
	m_swapchainFramebuffers.Resize( m_swapchainImageViews.Length() );

	for ( int i = 0; i < m_swapchainImageViews.Length(); i++ ) {
		qpArray< VkImageView, 1 > attachments {
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

void qpVulkan::CreateCommandPool() {
	queueFamilyIndices_t queueFamilyIndices = FindQueueFamilies( m_physicalDevice );

	VkCommandPoolCreateInfo poolInfo {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.GetValue();

	if ( vkCreateCommandPool( m_device, &poolInfo, NULL, &m_commandPool ) != VK_SUCCESS ) {
		ThrowOnError( "Failed to create command pool!" );
	}
}

void qpVulkan::CreateCommandBuffers() {
	m_commandBuffers.Resize( MAX_FRAMES_IN_FLIGHT );

	VkCommandBufferAllocateInfo allocInfo {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_commandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandBufferCount = m_commandBuffers.Length();

	if ( vkAllocateCommandBuffers( m_device, &allocInfo, m_commandBuffers.Data() ) != VK_SUCCESS ) {
		ThrowOnError( "Failed to allocate command buffers!" );
	}
}

void qpVulkan::CreateSyncObjects() {
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
			ThrowOnError( qpFormat( "Failed to create image available semaphore for frame %d!", index ) );
		}

		if ( vkCreateSemaphore( m_device, &semaphoreInfo, NULL, &m_renderFinishedSemaphores[ index ] ) != VK_SUCCESS ) {
			ThrowOnError( qpFormat( "Failed to create render finished semaphore for frame %d!", index ) );
		}

		if ( vkCreateFence( m_device, &fenceInfo, NULL, &m_inFlightFences[ index ] ) != VK_SUCCESS ) {
			ThrowOnError( qpFormat( "Failed to create in flight fence for frame %d!", index ) );
		}
	}
}

void qpVulkan::CleanupSwapchain() {
	for ( VkFramebuffer framebuffer : m_swapchainFramebuffers ) {
		vkDestroyFramebuffer( m_device, framebuffer, NULL );
	}

	for ( VkImageView imageView : m_swapchainImageViews ) {
		vkDestroyImageView( m_device, imageView, NULL );
	}

	vkDestroySwapchainKHR( m_device, m_swapchain, NULL );
}

void qpVulkan::CleanupSyncObjects() {
	for ( int index = 0; index < MAX_FRAMES_IN_FLIGHT; index++ ) {
		vkDestroySemaphore( m_device, m_imageAvailableSemaphores[ index ], NULL );
		vkDestroySemaphore( m_device, m_renderFinishedSemaphores[ index ], NULL );
		vkDestroyFence( m_device, m_inFlightFences[ index ], NULL );
	}
}

void qpVulkan::RecreateSwapchain() {
	vkDeviceWaitIdle( m_device );

	CleanupSyncObjects();
	CleanupSwapchain();

	CreateSwapchain();
	CreateImageViews();
	CreateFrameBuffers();
	CreateSyncObjects();
}

void qpVulkan::RecordCommandBuffer( VkCommandBuffer commandBuffer, int imageIndex ) {
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

	vkCmdBindDescriptorSets( commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSets[ m_currentFrame ], 0, NULL );

	vkCmdDrawIndexed( commandBuffer, meshIndices.Length(), 1, 0, 0, 0 );

	vkCmdEndRenderPass( commandBuffer );

	if ( vkEndCommandBuffer( commandBuffer ) != VK_SUCCESS ) {
		ThrowOnError( "Failed to record command buffer!" );
	}
}

void qpVulkan::CreateVertexBuffer() {
	VkDeviceSize bufferSize = sizeof( meshVertices[ 0 ] ) * meshVertices.Length();

	VkBuffer stagingBuffer = NULL;
	VkDeviceMemory stagingBufferMemory = NULL;
	CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );

	void * data = NULL;
	vkMapMemory( m_device, stagingBufferMemory, 0, bufferSize, 0, &data );
	memcpy( data, meshVertices.Data(), bufferSize );
	vkUnmapMemory( m_device, stagingBufferMemory );

	CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory );

	CopyBuffer( stagingBuffer, m_vertexBuffer, bufferSize );
	vkDestroyBuffer( m_device, stagingBuffer, NULL );
	vkFreeMemory( m_device, stagingBufferMemory, NULL );
}

void qpVulkan::CreateIndexBuffer() {
	VkDeviceSize bufferSize = sizeof( meshIndices[ 0 ] ) * meshIndices.Length();

	VkBuffer stagingBuffer;
	VkDeviceMemory stagingBufferMemory;
	CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory );

	void * data;
	vkMapMemory( m_device, stagingBufferMemory, 0, bufferSize, 0, &data );
	memcpy( data, meshIndices.Data(), bufferSize );
	vkUnmapMemory( m_device, stagingBufferMemory );

	CreateBuffer( bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory );

	CopyBuffer( stagingBuffer, m_indexBuffer, bufferSize );

	vkDestroyBuffer( m_device, stagingBuffer, NULL );
	vkFreeMemory( m_device, stagingBufferMemory, NULL );
}

void qpVulkan::CreateUniformBuffers() {
	VkDeviceSize bufferSize = sizeof( uniformBufferObject_t );

	m_uniformBuffers.Resize( MAX_FRAMES_IN_FLIGHT );
	m_uniformBuffersMemory.Resize( MAX_FRAMES_IN_FLIGHT );
	m_uniformBuffersMapped.Resize( MAX_FRAMES_IN_FLIGHT );

	for ( int index = 0; index < MAX_FRAMES_IN_FLIGHT; index++ ) {
		CreateBuffer( bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffers[ index ], m_uniformBuffersMemory[ index ] );

		vkMapMemory( m_device, m_uniformBuffersMemory[ index ], 0, bufferSize, 0, &m_uniformBuffersMapped[ index ] );
	}
}

void qpVulkan::CreateDescriptorPool() {
	VkDescriptorPoolSize poolSize {};
	poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSize.descriptorCount = static_cast< uint32 >( MAX_FRAMES_IN_FLIGHT );

	VkDescriptorPoolCreateInfo poolInfo {};
	poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolInfo.poolSizeCount = 1;
	poolInfo.pPoolSizes = &poolSize;
	poolInfo.maxSets = static_cast< uint32_t >( MAX_FRAMES_IN_FLIGHT );

	if ( vkCreateDescriptorPool( m_device, &poolInfo, NULL, &m_descriptorPool ) != VK_SUCCESS ) {
		ThrowOnError( "failed to create descriptor pool!" );
	}
}

void qpVulkan::CreateDescriptorSets() {
	qpList< VkDescriptorSetLayout > layouts( MAX_FRAMES_IN_FLIGHT, m_descriptorSetLayout );
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

		VkWriteDescriptorSet descriptorWrite {};
		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = m_descriptorSets[ index ];
		descriptorWrite.dstBinding = 0;
		descriptorWrite.dstArrayElement = 0;
		descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorWrite.descriptorCount = 1;
		descriptorWrite.pBufferInfo = &bufferInfo;
		descriptorWrite.pImageInfo = NULL;
		descriptorWrite.pTexelBufferView = NULL;

		vkUpdateDescriptorSets( m_device, 1, &descriptorWrite, 0, NULL);

	}

}

void qpVulkan::CreateBuffer( VkDeviceSize size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags propertyFlags, VkBuffer& outBuffer, VkDeviceMemory& outBufferMemory ) {
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

void qpVulkan::CopyBuffer( VkBuffer sourceBuffer, VkBuffer destinationBuffer, VkDeviceSize size ) {
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

	VkBufferCopy copyRegion {};
	copyRegion.srcOffset = 0;
	copyRegion.dstOffset = 0;
	copyRegion.size = size;
	vkCmdCopyBuffer( commandBuffer, sourceBuffer, destinationBuffer, 1, &copyRegion );

	vkEndCommandBuffer( commandBuffer );

	VkSubmitInfo submitInfo {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit( m_graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE );
	vkQueueWaitIdle( m_graphicsQueue );

	vkFreeCommandBuffers( m_device, m_commandPool, 1, &commandBuffer );
}

uint32 qpVulkan::FindMemoryType( uint32 typeFilter, VkMemoryPropertyFlags properties ) {
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

void UpdateUniformBuffer( void * mappedUBO, void * windowHandle ) {
	static qpTimePoint startTime = qpClock::Now();
	static qpTimePoint lastTime = startTime;

	qpTimePoint currentTime = qpClock::Now();
	qpTimePoint timeDiff = ( currentTime - startTime );
	qpTimePoint timeDelta = ( currentTime - lastTime );
	lastTime = currentTime;
	//const float time = timeDiff.AsSeconds();
	const float deltaTime = timeDelta.AsSeconds();

	int width;
	int height;
	GetWindowFramebufferSize( windowHandle, width, height );

	static qpVec3 translation( 0.0f, 0.0f, 50.0f );
	static qpVec3 rotation( 0.0f, 0.0f, 0.0f );


	if ( windowForTesting->GetKeyboard().IsKeyDown( keyboardKeys_t::KEY_X ) ) {
		rotation.y += 45.0f * deltaTime;
	}
	if ( windowForTesting->GetKeyboard().IsKeyDown( keyboardKeys_t::KEY_Z ) ) {
		rotation.y -= 45.0f * deltaTime;
	}

	if ( windowForTesting->GetKeyboard().IsKeyDown( keyboardKeys_t::KEY_C ) ) {
		rotation.x += 45.0f * deltaTime;
	}
	if ( windowForTesting->GetKeyboard().IsKeyDown( keyboardKeys_t::KEY_V ) ) {
		rotation.x -= 45.0f * deltaTime;
	}

	if ( windowForTesting->GetKeyboard().IsKeyDown( keyboardKeys_t::KEY_B ) ) {
		rotation.z += 45.0f * deltaTime;
	}
	if ( windowForTesting->GetKeyboard().IsKeyDown( keyboardKeys_t::KEY_N ) ) {
		rotation.z -= 45.0f * deltaTime;
	}

	qpQuat orientation( rotation.x, rotation.y, rotation.z ); //qpCreateRotationY( rotation.y ) * qpCreateRotationX( rotation.x );

	const float fwdSpeed = 100.0f;
	const float rightSpeed = 100.0f;
	const float upSpeed = 100.0f;
	float forwardDir = 0.0f;
	if ( windowForTesting->GetKeyboard().IsKeyDown( keyboardKeys_t::KEY_W ) ) {
		forwardDir = 1.0f;
	} else if ( windowForTesting->GetKeyboard().IsKeyDown( keyboardKeys_t::KEY_S ) ) {
		forwardDir = -1.0f;
	}
	float rightDir = 0.0f;
	if ( windowForTesting->GetKeyboard().IsKeyDown( keyboardKeys_t::KEY_D ) ) {
		rightDir = 1.0f;
	} else if ( windowForTesting->GetKeyboard().IsKeyDown( keyboardKeys_t::KEY_A ) ) {
		rightDir = -1.0f;
	}

	float upDir = 0.0f;
	if ( windowForTesting->GetKeyboard().IsKeyDown( keyboardKeys_t::KEY_Q ) ) {
		upDir = -1.0f;
	} else if ( windowForTesting->GetKeyboard().IsKeyDown( keyboardKeys_t::KEY_E ) ) {
		upDir = 1.0f;
	}
	translation += orientation.Forward() * fwdSpeed * deltaTime * forwardDir;

	translation += orientation.Right() * rightSpeed * deltaTime * rightDir;

	translation += g_vec3Up * upSpeed * deltaTime * upDir;

	if ( windowForTesting->GetKeyboard().IsKeyPressed( keyboardKeys_t::KEY_ENTER ) ) {
		translation = qpVec3( 0.0f, 0.0f, -50.0f );
	}

	uniformBufferObject_t ubo {};
	ubo.model = ( qpCreateRotationY( 180.0f ) * qpCreateTranslation( qpVec3( 0.0f, 0.0f, 200.0f ) ) ).Transposed();
	ubo.view = qpRotationAndTranslationInverse( orientation.ToMat4() * qpCreateTranslation( translation ) ).Transposed();
	ubo.projection = qpPerspectiveProjectionMatrix( 90.0f, width, height, 1.0f, 100000.0f ).Transposed();

	qpCopy( static_cast< uniformBufferObject_t * >( mappedUBO ), 1, &ubo, 1 );
}

void qpVulkan::DrawFrame() {
	int width;
	int height;

	GetWindowFramebufferSize( m_windowHandle, width, height );

	if ( width == 0 || height == 0 ) {
		return;
	}

	vkWaitForFences( m_device, 1, &m_inFlightFences[ m_currentFrame ], VK_TRUE, UINT64_MAX );

	uint32 imageIndex;
	VkResult result = vkAcquireNextImageKHR( m_device, m_swapchain, UINT64_MAX, m_imageAvailableSemaphores[ m_currentFrame ], VK_NULL_HANDLE, &imageIndex );

	if( ( result == VK_ERROR_OUT_OF_DATE_KHR ) || m_framebufferResized ) {
		m_framebufferResized = false;
		RecreateSwapchain();
		return;
	}

	if( result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR ) {
		ThrowOnError( qpFormat( "vkAcquireNextImageKHR failed with error code: %d", result ) );
	}

	vkResetFences( m_device, 1, &m_inFlightFences[ m_currentFrame ] );

	vkResetCommandBuffer( m_commandBuffers[ m_currentFrame ], 0 );

	RecordCommandBuffer( m_commandBuffers[ m_currentFrame ], imageIndex );

	UpdateUniformBuffer( m_uniformBuffersMapped[ m_currentFrame ], m_windowHandle );

	VkSubmitInfo submitInfo {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	qpArray< VkSemaphore, 1 > waitSemaphores {
		m_imageAvailableSemaphores[ m_currentFrame ]
	};

	qpArray< VkPipelineStageFlags, 1 > waitStages {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	};
	submitInfo.waitSemaphoreCount = waitSemaphores.Length();
	submitInfo.pWaitSemaphores = waitSemaphores.Data();
	submitInfo.pWaitDstStageMask = waitStages.Data();
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_commandBuffers[ m_currentFrame ];

	qpArray< VkSemaphore, 1 > signalSemaphores {
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

void qpVulkan::RequestFramebufferResize() {
	m_framebufferResized = true;
}

bool qpVulkan::CheckValidationLayerSupport( const qpArrayView< const char * > & layersView ) {
	uint32 count;
	vkEnumerateInstanceLayerProperties( &count, NULL );

	qpList< VkLayerProperties > availableLayers( count );
	vkEnumerateInstanceLayerProperties( &count, availableLayers.Data() );

	for ( const char * layerName : layersView ) {
		bool found = false;

		for ( const VkLayerProperties & layerProperties : availableLayers ) {
			if ( qpStrEquals( layerName, layerProperties.layerName ) ) {
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

void qpVulkan::ThrowOnError( const qpString & msg ) {
#if defined( QP_PLATFORM_WINDOWS )
	MessageBox( NULL, qpUTF8ToWide( msg ).c_str(), L"qp", MB_ICONERROR );
#else
	qpDebug::Error( "%s", msg.c_str() );
#endif
	throw std::runtime_error( msg.c_str() );
}
