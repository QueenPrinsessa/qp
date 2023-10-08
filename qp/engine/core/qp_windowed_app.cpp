#include "qp_windowed_app.h"
#include "qp/engine/debug/qp_debug.h"
#include <functional>
#ifdef QP_PLATFORM_WINDOWS
#include "qp/engine/platform/windows/window/qp_window_windows.h"
#endif

qpWindowedApp::qpWindowedApp( const qpWindowProperties_t & windowProperties ) {
	m_windowProperties = windowProperties;
}

void qpWindowedApp::OnInit() {
	qpDebug::InitializeConsole();

#ifdef QP_PLATFORM_WINDOWS
	m_window = qpCreateUnique< qpWindowsWindow >( m_windowProperties );
#endif
	m_window->SetDestroyCallback( [ & ] () {
		RequestShutdown();
	} );
	m_window->SetResizeCallback( [ & ] ( int width, int height ) {
		qpDebug::Info( "Window resized to ( w:%d , h:%d ).", width, height );
		m_vulkan->RequestFramebufferResize();
	} );
	m_vulkan = qpCreateUnique< qpVulkan >();

	m_vulkan->Init( m_window->GetHandle() );
}

void qpWindowedApp::OnUpdate() {
	m_vulkan->DrawFrame();
	m_window->OnUpdate();
}

void qpWindowedApp::OnCleanup() {
	m_vulkan.Reset();
	m_window.Reset();
}
