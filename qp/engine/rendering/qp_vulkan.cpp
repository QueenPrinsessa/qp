#include "qp_vulkan.h"
#include "qp/tools/containers/qp_array.h"
#include "qp/tools/containers/qp_list.h"
#include "vulkan/vulkan.h"
#include <array>
#include <stdexcept>
#include <iostream>

qpVulkan::qpVulkan() {
}

qpVulkan::~qpVulkan() {
    Cleanup();
}

void qpVulkan::Init() {
    CreateInstance();
}

void qpVulkan::Cleanup() {
    vkDestroyInstance( m_instance, NULL );
}

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

    createInfo.enabledExtensionCount = enabledExtensions.Length();
    createInfo.ppEnabledExtensionNames = enabledExtensions.Data();

    qpArray< const char *, 1 > validationLayers {
        "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    const bool enableValidationLayers = false;
#else
    const bool enableValidationLayers = true;
#endif
    
    VkResult result = vkCreateInstance( &createInfo, NULL, &m_instance );

    if( result == VK_ERROR_INCOMPATIBLE_DRIVER ) {
        ThrowOnError( "Driver is incompatible with Vulkan. vkCreateInstance failed." );
    } else if ( result != VK_SUCCESS ) {
        ThrowOnError( "vkCreateInstance failed." );
    }
}

void qpVulkan::ThrowOnError( const qpString & msg ) {
#ifdef QP_PLATFORM_WINDOWS
    MessageBox( NULL, qpUTF8ToWide( msg ).c_str(), L"qp", MB_ICONERROR );
#else
    std::cerr << msg << std::endl;
#endif
    throw std::runtime_error( msg.c_str() );
}
