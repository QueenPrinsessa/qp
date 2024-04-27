#include "engine.pch.h"
#include "qp_windowed_app.h"
#include "qp/engine/rendering/qp_vulkan.h"
#if defined( QP_PLATFORM_WINDOWS )
#include "qp/engine/platform/windows/window/qp_window_win32.h"
#else
#error "Window include not added for platform!"
#endif

qpWindowedApp::qpWindowedApp( const qpWindowProperties_t & windowProperties ) {
	m_windowProperties = windowProperties;
}

void qpWindowedApp::OnInit() {
#if defined( QP_PLATFORM_WINDOWS )
	m_window = qpCreateUnique< qpWindow_Win32 >( m_windowProperties );
#else
#error "Window creation not setup for platform!"
#endif
	m_window->SetDestroyCallback( [ & ] () {
		RequestShutdown();
	} );
	m_window->SetResizeCallback( [ & ] ( int width, int height ) {
		qpLog::Info( "Window resized to ( w:%d , h:%d ).", width, height );
		m_vulkan->RequestFramebufferResize();
	} );
	m_vulkan = qpCreateUnique< qpVulkan >();

	m_vulkan->Init( m_window->GetHandle(), m_window.Raw() );
}

void qpWindowedApp::OnUpdate() {
	m_vulkan->DrawFrame();
	m_window->OnUpdate();
}

void qpWindowedApp::OnCleanup() {
	m_vulkan.Reset();
	m_window.Reset();
}
