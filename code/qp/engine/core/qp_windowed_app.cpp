#include "engine.pch.h"
#include "qp_windowed_app.h"
#include "qp/common/time/qp_clock.h"
#include "qp/engine/debug/qp_debug.h"
#include "qp/engine/resources/image/qp_image.h"
#include "qp/engine/resources/loaders/qp_tga_loader.h"
#if defined( QP_PLATFORM_WINDOWS )
#include "qp/engine/platform/windows/window/qp_window_windows.h"
#endif

qpWindowedApp::qpWindowedApp( const qpWindowProperties_t & windowProperties ) {
	m_windowProperties = windowProperties;
}

void qpWindowedApp::OnInit() {
	qpDebug::InitializeConsole();

#if defined( QP_PLATFORM_WINDOWS )
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

	m_vulkan->Init( m_window->GetHandle(), m_window.Raw() );
}

void qpWindowedApp::OnUpdate() {
	m_vulkan->DrawFrame();
	m_window->OnUpdate();

	//static qpTimePoint timeLast = qpClock::Now();
	//qpTimePoint time = qpClock::Now();
	//qpTimePoint deltaTime = ( time - timeLast );
	//timeLast = time;
	//qpDebug::Info( "Time: %lld", deltaTime.AsFPS() );
}

void qpWindowedApp::OnCleanup() {
	m_vulkan.Reset();
	m_window.Reset();

	qpResourceLoader::DeleteAllResources();
}
