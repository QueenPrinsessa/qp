#include "qp_vulkan.h"
#include "qp/engine/debug/qp_debug.h"
#include "qp/tools/containers/qp_array.h"
#include "qp/tools/containers/qp_list.h"
#include "vulkan/vulkan.h"
#include <stdexcept>
#include <iostream>

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

qpVulkan::qpVulkan() { }

qpVulkan::~qpVulkan() {
	Cleanup();
}

void qpVulkan::Init() {
	CreateInstance();
	SetupDebugMessenger();
	PickPhysicalDevice();
}

void DestroyDebugUtilsMessengerEXT( VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks * allocator );

void qpVulkan::Cleanup() {
	vkDestroyInstance( m_instance, NULL );

	if ( enableValidationLayers ) {
		DestroyDebugUtilsMessengerEXT( m_instance, m_debugMessenger, NULL );
	}
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
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT: {
			qpDebug::Log( "%s", callbackData->pMessage );
			break;
		}
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT: {
			qpDebug::Info( "%s", callbackData->pMessage );
			break;
		}
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT: {
			qpDebug::Warning( "%s", callbackData->pMessage );
			break;
		}
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT: {
			qpDebug::Error( "%s", callbackData->pMessage );
			break;
		}
		case VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT: {
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

	if ( CreateDebugUtilsMessengerEXT( m_instance, &createInfo, nullptr, &m_debugMessenger ) != VK_SUCCESS ) {
		ThrowOnError( "Failed to set up debug messenger!" );
	}
}

bool IsDeviceSuitable( VkPhysicalDevice device ) {
	UNUSED_PARAMETER( device );
	//VkPhysicalDeviceProperties deviceProperties;
	//vkGetPhysicalDeviceProperties( device, &deviceProperties );

	//VkPhysicalDeviceFeatures deviceFeatures;
	//vkGetPhysicalDeviceFeatures( device, &deviceFeatures );

	return true;
}

void qpVulkan::PickPhysicalDevice() {
	uint32 deviceCount = 0;
	vkEnumeratePhysicalDevices( m_instance, &deviceCount, NULL );

	if( deviceCount == 0 ) {
		ThrowOnError( "Couldn't find any GPUs with Vulkan support!" );
	}

	qpList< VkPhysicalDevice > devices( deviceCount );
	vkEnumeratePhysicalDevices( m_instance, &deviceCount, devices.Data() );

	for( VkPhysicalDevice device : devices ) {
		if( IsDeviceSuitable( device ) ) {
			m_physicalDevice = device;
			break;
		}
	}

	if( m_physicalDevice == NULL ) {
		ThrowOnError( "Couldn't find a suitable GPU!" );
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
