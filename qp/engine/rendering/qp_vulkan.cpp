#include "qp_vulkan.h"
#include "vulkan/vulkan.h"
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
    appInfo.apiVersion = VK_MAKE_VERSION( 1, 0, 3 );

    VkInstanceCreateInfo createInfo {};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &appInfo;

    const char * enabledExtensions[] {
        VK_KHR_SURFACE_EXTENSION_NAME,
#ifdef QP_PLATFORM_WINDOWS
        VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#endif
        VK_EXT_DEBUG_REPORT_EXTENSION_NAME,
    };

    createInfo.enabledExtensionCount = sizeof( enabledExtensions ) / sizeof( const char * );
    createInfo.ppEnabledExtensionNames = enabledExtensions;

    const char * validationLayers[] {
        "VK_LAYER_KHRONOS_validation"
    };

#ifdef NDEBUG
    constexpr bool enableValidationLayers = false;
#else
    constexpr bool enableValidationLayers = true;
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
