#include "game.pch.h"
#include "common/threads/qp_thread_pool.h"
#if defined( QP_HEADLESS )
#include "qp/engine/core/qp_headless_app.h"
#else
#include "qp/engine/core/qp_windowed_app.h"
#endif

class qpGameApp : public qpWindowedApp {
public:
	explicit qpGameApp ( const windowProperties_t & windowProperties )
		: qpWindowedApp( windowProperties ) {
	}
};

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/common/platform/windows/qp_types_win32.h"
#include "qp/engine/platform/windows/window/qp_window_win32.h"

int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow ) {
#if !defined( QP_RETAIL )
	if ( Sys_InitializeConsole() ) {
		qpDebug::Trace( "Successfully initialized console." );
	}
#endif

#if defined( QP_HEADLESS )
	qpHeadlessApp app;
#else
	windowProperties_t windowProperties;
	windowProperties.width = 800;
	windowProperties.height = 600;
	windowProperties.allowResize = true;
	windowProperties.mode = windowMode_t::WINDOWED;
#if defined( QP_VULKAN )
	windowProperties.title = "qpVulkan Window Win32";
#elif defined( QP_D3D11 )
	windowProperties.title = "qpD3D11 Window Win32";
#endif
	windowPropertiesWindows_t windowsProperties;
	windowsProperties.instanceHandle = hInstance;

	windowProperties.platformData = &windowsProperties;

	qpGameApp app( windowProperties );
#endif

	qpThreadPool threadPool;
	threadPool.Startup( threadPool.MaxWorkers() );
	
	threadPool.QueueJob( []() { qpDebug::Printf( "I'm just thread I like to work :)\n" ); } );
	threadPool.QueueJob( []() { qpDebug::Printf( "Nooooo, I don't enjoy working >:(\n" ); } );
	try {
		app.Run();
	} catch ( const std::exception & e ) {
		qpDebug::Error( "%s", e.what() );
	}

	threadPool.Shutdown();
	qpDebug::Trace( "Shutting down." );

	qpDebug::FlushLogFile();
	
#if !defined( QP_RETAIL )
	// keep console open
	system( "pause" );
#endif
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
		qpDebug::Error( "%s", e.what() );
	}

	return 0;
}
#else
#error There's no entrypoint defined for platform!
#endif
