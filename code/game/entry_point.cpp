#include "game.pch.h"
#include <iostream>
#if defined( QP_HEADLESS )
#include "qp/engine/core/qp_headless_app.h"
#else
#include "qp/engine/core/qp_windowed_app.h"
#endif

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/common/platform/windows/qp_types_win32.h"
#include "qp/engine/platform/windows/window/qp_window_win32.h"

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow ) {
#if !defined( QP_RETAIL )
	Sys_InitializeConsole();
#endif

#if defined( QP_HEADLESS )
	qpHeadlessApp app;
#else
	windowProperties_t windowProperties;
	windowProperties.width = 800;
	windowProperties.height = 600;
	windowProperties.allowResize = true;
	windowProperties.mode = windowMode_t::WINDOWED;
	windowProperties.title = "qpVulkanWindowsWindow";

	windowPropertiesWindows_t windowsProperties;
	windowsProperties.instanceHandle = hInstance;

	windowProperties.platformData = &windowsProperties;

	qpWindowedApp app( windowProperties );
#endif

	try {
		app.Run();
	} catch ( const std::exception & e ) {
		qpLog::Error( "%s", e.what() );
	}

	return 0;
}

#elif defined( QP_PLATFORM_LINUX )
int main() {
#if !defined( QP_RETAIL )
	Sys_InitializeConsole();
#endif

#if defined( QP_HEADLESS )
	qpHeadlessApp app;
#else
	windowProperties_t windowProperties;
	windowProperties.width = 800;
	windowProperties.height = 600;
	windowProperties.allowResize = true;
	windowProperties.mode = windowMode_t::WINDOWED;
	windowProperties.title = "qpVulkanLinuxWindow";

	// windowPropertiesWindows_t windowsProperties;
	// windowsProperties.instanceHandle = hInstance;

	//windowProperties.platformData = &windowsProperties;

	qpWindowedApp app( windowProperties );
#endif

	try {
		app.Run();
	} catch ( const std::exception & e ) {
		qpLog::Error( "%s", e.what() );
	}

	return 0;
}
#else
#error There's no entrypoint defined for platform!
#endif
