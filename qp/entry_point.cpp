#include "common/string/qp_string.h"
#include <chrono>
#include <iostream>
#include "engine/core/qp_app.h"
#include "engine/core/qp_windowed_app.h"

#ifdef QP_PLATFORM_WINDOWS

#include "qp/engine/platform/windows/window/qp_window_windows.h"

int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow ) {
	qpWindowProperties_t windowProperties;
	windowProperties.width = 800;
	windowProperties.height = 600;
	windowProperties.allowResize = true;
	windowProperties.mode = qpWindowMode_t::WINDOWED;
	windowProperties.title = "qpVulkanWindowsWindow";

	qpWindowPropertiesWindows_t windowsProperties;
	windowsProperties.instanceHandle = hInstance;

	windowProperties.platformData = &windowsProperties;

	qpWindowedApp app( windowProperties );

	try {
		app.Run();
	} catch ( const std::exception & e ) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

#endif
