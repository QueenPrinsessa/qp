#include "qp_vulkan.h"
#include "qp/engine/debug/qp_debug.h"
#include "qp/tools/containers/qp_array.h"
#include "qp/tools/containers/qp_list.h"
#include "qp/tools/containers/qp_set.h"
#include "vulkan/vulkan.h"
#include <stdexcept>
#include <iostream>
#include <qp/common/utilities/qp_optional.h>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

static qpList< const char * > deviceExtensions {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

qpVulkan::qpVulkan() { }

qpVulkan::~qpVulkan() {
	Cleanup();
}

void qpVulkan::Init( void * windowHandle ) {
	m_windowHandle = windowHandle;
	CreateInstance();
	SetupDebugMessenger();
	CreateSurface( windowHandle );
	PickPhysicalDevice();
	CreateLogicalDevice();
}

void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks * allocator );

void qpVulkan::Cleanup() {

	for ( auto imageView : m_swapchainImageViews ) {
		vkDestroyImageView( m_device, imageView, NULL );
	}

	vkDestroySwapchainKHR( m_device, m_swapchain, NULL );

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

#ifdef QP_PLATFORM_WINDOWS
	enabledExtensions.Push( VK_KHR_WIN32_SURFACE_EXTENSION_NAME );
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
		vkGetDeviceQueue( m_device, indices.presentFamily.GetValue(), 0, &m_graphicsQueue );
	}
}

void qpVulkan::CreateSurface( void * m_windowHandle ) {

#ifdef QP_PLATFORM_WINDOWS
	VkWin32SurfaceCreateInfoKHR createInfo {};
	createInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = static_cast< HWND >( m_windowHandle );
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

#ifdef QP_PLATFORM_WINDOWS
	RECT rect;
	GetClientRect( static_cast< HWND >( m_windowHandle ), &rect );

	width = rect.right - rect.left;
	height = rect.bottom - rect.top;
#endif

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
#ifdef QP_PLATFORM_WINDOWS
	MessageBox( NULL, qpUTF8ToWide( msg ).c_str(), L"qp", MB_ICONERROR );
#else
	qpDebug::Error( "%s", msg.c_str() );
#endif
	throw std::runtime_error( msg.c_str() );
}
