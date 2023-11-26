
vulkan_sdk = os.getenv("VULKAN_SDK")

include_dirs = {}
include_dirs["vulkan"] = "%{vulkan_sdk}/include"

lib_dirs = {}
lib_dirs["vulkan"] = "%{vulkan_sdk}/lib"

libs = {}
libs["vulkan"] = "vulkan-1.lib"