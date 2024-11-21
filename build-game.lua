project "game"
location( slnDir .. "projects/" ) 
kind "WindowedApp"
language "C++"
cppdialect "C++20"
staticruntime "Off"

debugdir(gamedir)
targetdir (bindir .."/" .. outputdir .. "%{prj.name}")
objdir (tempdir .. "/" .. outputdir .. "/%{prj.name}")

game_src_dir = codedir .. "game/"

pchheader ( "game.pch.h" )
pchsource ( game_src_dir .. "game.pch.h.cpp" )

defines {
    "_CRT_SECURE_NO_WARNINGS"
}

files {
    game_src_dir .. "**.h",
    game_src_dir .. "**.cpp",
    game_src_dir .. "**.hpp",
    game_src_dir .. "**.inl"
}

includedirs {
    codedir,
    game_src_dir,
    engine_src_dir,
    include_dirs["qp_ecs"]
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

filter { "platforms:*_d3d11_*" }
    links {
        "d3d11.lib",
        "dxgi.lib",
        "d3dcompiler.lib"
    }