include("dependencies.lua")

workspace "qp"
    location "."
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
        "windows_d3d11_dev_x64",
        "windows_d3d11_x64",
        "windows_vulkan_dev_x64",
        "windows_vulkan_x64",
        "windows_headless_dev_x64",
	    "windows_headless_x64",
        "linux_vulkan_dev_x64",
        "linux_vulkan_x64",
        "linux_headless_dev_x64",
	    "linux_headless_x64",
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
        toolset "msc"
    filter { "system:linux" }
        systemversion "latest"
        defines {
            "QP_PLATFORM_LINUX"
        }
        toolset "clang"
        toolchainversion "wsl2"
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
    filter { "platforms:linux_*" }
        system "linux"
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
    filter { "platforms:*_d3d11_*" }
        defines {
            "QP_D3D11"
        }
    filter { "platforms:*_vulkan_*" }
        defines {
            "QP_VULKAN"
        }
        externalincludedirs {
            include_dirs["vulkan"]
        }
    filter { "platforms:*_headless_*" }
        defines {
            "QP_HEADLESS"
        }
    filter { "toolset:msc" }
        disablewarnings {
            "4100", -- unreferenced formal parameter
            "4201", -- nonstandard extension used: nameless struct/union
            "4505" -- unreferenced local function has been removed
        }
        flags {
            "MultiProcessorCompile",
            "FatalCompileWarnings"
        }
    filter { "toolset:clang" }
        disablewarnings {
            "unused-function"
        }
        flags {}
    filter {}

slnDir = "%{wks.location}/"
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"
codedir = slnDir .. "code/"
bindir = slnDir .. "bin/"
tempdir = slnDir .. "temp/"
gamedir = slnDir .. "game/"

include("build-engine.lua")
include("build-game.lua")
