project "game"
location "."
kind "WindowedApp"
language "C++"
cppdialect "C++20"
staticruntime "Off"

debugdir(gamedir)
targetdir (bindir .."/" .. outputdir .. "%{prj.name}")
objdir (tempdir .. "/" .. outputdir .. "/%{prj.name}")

pchheader "game.pch.h"
pchsource "game.pch.h.cpp"

disablewarnings {
    "4100", -- unreferenced formal parameter
    "4201" -- nonstandard extension used: nameless struct/union
}

defines {
    "_CRT_SECURE_NO_WARNINGS"
}

flags {
    "MultiProcessorCompile",
    "FatalCompileWarnings"
}

files {
    "**.h",
    "**.cpp",
    "**.hpp",
    "**.inl"
}

includedirs {
    codedir,
    "%{codedir}/qp",
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
