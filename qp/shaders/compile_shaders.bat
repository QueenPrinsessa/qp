@echo off
set shaderExtensions=.vert .frag

set outputDir=spir-v

if not exist %outputDir% (
	mkdir %outputDir%
)

( for %%e in ( %shaderExtensions% ) do ( 
	( for %%f in ( *%%e ) do (
		%Vulkan_SDK%/bin/glslc.exe %%f -o %outputDir%/%%f.spv
		echo compiled %%f to %outputDir%/%%f.spv
	) )
))	

pause

