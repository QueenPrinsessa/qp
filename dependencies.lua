
vulkan_sdk = os.getenv("VULKAN_SDK")

sdl_version = "3.1.6"

include_dirs = {}
include_dirs["vulkan"] = "%{vulkan_sdk}/include"
include_dirs["qp_ecs"] = "%{libsdir}/QPEcs/include"
include_dirs["sdl"] = "%{libsdir}/SDL/" .. sdl_version .. "/include"

lib_dirs = {}
lib_dirs["vulkan"] = "%{vulkan_sdk}/lib"
lib_dirs["sdl_debug"] = "%{libsdir}/SDL/" .. sdl_version .. "/lib/debug"
lib_dirs["sdl"] = "%{libsdir}/SDL/" .. sdl_version .. "/lib"

libs = {}
libs["vulkan"] = "vulkan-1.lib"
libs["sdl"] = "SDL3"
