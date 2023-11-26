include("dependencies.lua")

workspace "qp"
    architecture "x64"
    startproject "game"
    configurations {
        "debug", -- internal build with full symbols
        "release", -- optimized internal build with symbols
        "retail" -- optimized without symbols, for distribution.
    }
    --platform naming:
    --dev builds: platform_graphics_dev_architecture
    --builds: platform_graphics_architectures
    platforms {
        "windows_vulkan_dev_x64",
        "windows_vulkan_x64",
        -- "linux_vulkan_dev_x64",
        -- "linux_vulkan_x64",
    }
    filter { "system:windows" }
        systemversion "latest"
        buildoptions { 
            "/EHsc", --iso standard exception handling
            "/Zc:preprocessor", --enable conforming preprocessor for msvc 
            "/Zc:__cplusplus" --__cplusplus version macro 
        } 
        defines {
            "QP_PLATFORM_WINDOWS"
        }
    -- filter { "system:linux" }
    --     systemversion "latest"
    --     defines {
    --         "QP_PLATFORM_LINUX"
    --     }
    filter { "configurations:debug" }
        defines {
            "QP_DEBUG"
        }
        runtime "debug"
        symbols "on" 
    filter { "configurations:release" }
        defines {
            "QP_RELEASE"
        }
        runtime "release"
        optimize "on"
        symbols "on" 
    filter { "configurations:retail" }
        defines {
            "QP_RETAIL"
        }
        runtime "release"
        optimize "on"
        symbols "off"
    filter { "platforms:windows_*" }
        system "windows"
    -- filter { "platforms:linux_*" }
    --     system "linux"
    filter { "platforms:*_x64" }
        architecture "x86_64"
    filter { "platforms:windows_*", "platforms:*_vulkan_*" }
        defines {
            "VK_USE_PLATFORM_WIN32_KHR"
        }
    filter { "platforms:*_dev_*" }
        defines {
            "QP_DEV"
        }
    filter { "platforms:*_vulkan_*" }
        externalincludedirs {
            include_dirs["vulkan"]
        }
    filter {}

outputdir = "code/%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
codedir = "%{wks.location}/code"
bindir = "%{wks.location}/bin"
tempdir = "%{wks.location}/temp"
gamedir = "%{wks.location}/game"

include("code/qp/build-engine.lua")
include("code/game/build-game.lua")