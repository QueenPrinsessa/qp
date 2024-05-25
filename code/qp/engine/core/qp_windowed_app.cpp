#include "engine.pch.h"

#if !defined( QP_HEADLESS )

#if defined( QP_D3D11 )
#include "engine/rendering/qp_d3d11.h"
#elif defined( QP_VULKAN )
#include "engine/rendering/qp_vulkan.h"
#else
#error "Rendering include missing for graphics api!"
#endif

#include "qp_windowed_app.h"

#if defined( QP_PLATFORM_WINDOWS )
#include "qp/engine/platform/windows/window/qp_window_win32.h"
#else
#error "Window include not added for platform!"
#endif

qpWindowedApp::qpWindowedApp( const windowProperties_t & windowProperties ) {
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
		qpDebug::Info( "Window resized to ( w:%d , h:%d ).", width, height );
		m_graphicsAPI->RequestFramebufferResize();
	} );

#if defined( QP_D3D11 )
	m_graphicsAPI = qpCreateUnique< qpD3D11 >();
#elif defined( QP_VULKAN )
	m_graphicsAPI = qpCreateUnique< qpVulkan >();

	// todo: remove test code.
	static_cast< qpVulkan * >( m_graphicsAPI.Raw() )->SetTestWindow(m_window.Raw());
#endif

	m_graphicsAPI->Init( m_window->GetHandle() );
}

void qpWindowedApp::OnUpdate() {
	m_graphicsAPI->DrawFrame();
	m_window->OnUpdate();
}

void qpWindowedApp::OnCleanup() {
	m_graphicsAPI.Reset();
	m_window.Reset();
}

#endif 