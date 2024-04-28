project "game"
location( slnDir .. "projects/" ) 
kind "WindowedApp"
language "C++"
cppdialect "C++20"
staticruntime "Off"

debugdir(gamedir)
targetdir (bindir .."/" .. outputdir .. "%{prj.name}")
objdir (tempdir .. "/" .. outputdir .. "/%{prj.name}")

pchheader "game.pch.h"
pchsource "game.pch.h.cpp"

defines {
    "_CRT_SECURE_NO_WARNINGS"
}

files {
    "**.h",
    "**.cpp",
    "**.hpp",
    "**.inl"
}

includedirs {
    codedir,
    codedir .. "/qp/",
    include_dirs["vulkan"]
}

links {
    "qp"
}

externalincludedirs {}

externalwarnings "Off"
warnings "Extra"

filter { "platforms:*_vulkan_*" }
    libdirs {
        lib_dirs["vulkan"]
    }
    links {
        libs["vulkan"]
    }
