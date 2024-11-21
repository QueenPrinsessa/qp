project "qp"
location( slnDir .. "projects/" ) 
kind "StaticLib"
language "C++"
cppdialect "C++20"
staticruntime "Off"

targetdir (bindir .. "/" .. outputdir .. "/%{prj.name}")
objdir (tempdir .. "/" .. outputdir .. "/%{prj.name}")

pchheader ( "engine.pch.h" )
pchsource ( codedir .. "qp/engine.pch.h.cpp" )

linkoptions {} 

defines {
    "_CRT_SECURE_NO_WARNINGS"
}

engine_src_dir = codedir .. "qp/"

files {
    engine_src_dir .. "**.h",
    engine_src_dir .. "**.cpp",
    engine_src_dir .. "**.hpp",
    engine_src_dir .. "**.inl",
}

includedirs {
    codedir,
    engine_src_dir,
    include_dirs["qp_ecs"]
}

externalwarnings "Off"
warnings "Extra"

