#include <vulkan/vulkan.hpp>
#include <iostream>

int main() {

    uint32_t extensionCount = 0;
    vkEnumerateInstanceExtensionProperties( nullptr, &extensionCount, nullptr );

    std::cout << extensionCount << " extensions supported\n";

    return 0;
}