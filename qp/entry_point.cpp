#include "common/core/qp_unique_ptr.h"
#include "engine/qp_app.h"
#include "tools/string/qp_string.h"
#include <chrono>
#include <iostream>

#ifdef QP_PLATFORM_WINDOWS

#include "qp/engine/platform/windows/window/qp_window_windows.h"

int WINAPI wWinMain( _In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ PWSTR pCmdLine, _In_ int nCmdShow ) {
	qpWindowProperties_t windowProperties;
	windowProperties.width = 1280;
	windowProperties.height = 720;
	windowProperties.allowResize = true;
	windowProperties.mode = qpWindowMode_t::WINDOWED;
	windowProperties.title = "qpVulkanWindowsWindow";

	qpWindowPropertiesWindows_t windowsProperties;
	windowsProperties.instanceHandle = hInstance;

	windowProperties.platformData = &windowsProperties;

	class qpTestApp : public qpApp {
	public:
		qpUniquePtr< qpWindow > window = NULL;
		qpWindowProperties_t windowProperties;

		qpTestApp( const qpWindowProperties_t & props ) {
			windowProperties = props;
		}

		std::unique_ptr< qpWindowsWindow > def;
		virtual void OnInit() override {
			window = qpCreateUnique< qpWindowsWindow >( windowProperties );
		}

		virtual void OnUpdate() override {
			//std::cout << __FUNCTION__ << std::endl;
			window->OnUpdate();
		}

		virtual void OnCleanup() override {
		}
	};

	qpTestApp app( windowProperties );

	try {
		app.Run();
	} catch ( const std::exception & e ) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

#endif
