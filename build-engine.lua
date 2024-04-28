project "qp"
location( slnDir .. "projects/" ) 
kind "StaticLib"
language "C++"
cppdialect "C++20"
staticruntime "Off"

targetdir (bindir .. "/" .. outputdir .. "/%{prj.name}")
objdir (tempdir .. "/" .. outputdir .. "/%{prj.name}")

pchheader "engine.pch.h"
pchsource "engine.pch.h.cpp"

linkoptions {} 

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
    codedir .. "/qp/"
}

externalwarnings "Off"
warnings "Extra"

