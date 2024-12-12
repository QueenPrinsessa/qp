#include "engine.pch.h"
#include "common/time/qp_clock.h"
#include "engine/rendering/qp_render_scene.h"

#if !defined( QP_HEADLESS )

#if defined( QP_VULKAN )
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

namespace qp {
	WindowedApp::WindowedApp( const windowProperties_t & windowProperties ) {
		m_windowProperties = windowProperties;
	}

	void WindowedApp::OnInit() {
#if defined( QP_PLATFORM_WINDOWS )
		m_window = CreateUnique< Window_Win32 >( m_windowProperties );
#else
#error "Window creation not setup for platform!"
#endif
		m_window->SetDestroyCallback( [ & ]() {
			RequestShutdown();
		} );
		m_window->SetResizeCallback( [ & ]( int width, int height ) {
			debug::Info( "Window resized to ( w:%d , h:%d ).", width, height );
			m_graphicsAPI->RequestFramebufferResize();
		} );

#if defined( QP_VULKAN )
		m_graphicsAPI = CreateUnique< Vulkan >();
#endif

		m_graphicsAPI->Init( m_window.Raw() );

		m_renderScene = CreateUnique< RenderScene >();

		// initializing to NaN for error checking.
		renderCamera_t camera { g_mat4NaN, g_mat4NaN };
		m_renderScene->SetRenderCamera( camera );
	}

	void WindowedApp::OnBeginFrame() {
		TimePoint currentTime = Clock::Now();
		m_deltaTime = ( currentTime - m_beginFrameTime );
		m_beginFrameTime = currentTime;
	}

	void WindowedApp::OnEndFrame() {
		const renderCamera_t & renderCamera = m_renderScene->GetRenderCamera();
		QP_ASSERT_RELEASE_MSG( renderCamera.projection != g_mat4NaN, "Render camera must be setup before we draw a frame!" );
		QP_ASSERT_RELEASE_MSG( renderCamera.view != g_mat4NaN, "Render camera must be setup before we draw a frame!" );
		m_graphicsAPI->DrawFrame( renderCamera );
		m_window->OnUpdate();

	}

	void WindowedApp::OnCleanup() {
		m_graphicsAPI.Reset();
		m_window.Reset();
	}
}
#endif 