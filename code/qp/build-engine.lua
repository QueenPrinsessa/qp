project "qp"
location "."
kind "StaticLib"
language "C++"
cppdialect "C++20"
staticruntime "Off"

targetdir (bindir .. "/" .. outputdir .. "/%{prj.name}")
objdir (tempdir .. "/" .. outputdir .. "/%{prj.name}")

pchheader "engine.pch.h"
pchsource "engine.pch.h.cpp"

linkoptions {} 

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
    codedir
}

externalwarnings "Off"
warnings "Extra"

