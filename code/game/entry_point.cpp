#include "game.pch.h"
#include <iostream>
#include "engine/core/qp_windowed_app.h"

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/common/platform/windows/qp_types_win32.h"
#include "qp/engine/platform/windows/window/qp_window_win32.h"

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow ) {
#if !defined( QP_RETAIL )
	Sys_InitializeConsole();
#endif

	qpWindowProperties_t windowProperties;
	windowProperties.width = 800;
	windowProperties.height = 600;
	windowProperties.allowResize = true;
	windowProperties.mode = windowMode_t::WINDOWED;
	windowProperties.title = "qpVulkanWindowsWindow";

	windowPropertiesWindows_t windowsProperties;
	windowsProperties.instanceHandle = hInstance;

	windowProperties.platformData = &windowsProperties;

	qpWindowedApp app( windowProperties );

	try {
		app.Run();
	} catch ( const std::exception & e ) {
		qpLog::Error( "%s", e.what() );
	}

	return 0;
}

#endif
