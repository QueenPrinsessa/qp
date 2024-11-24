﻿#include "engine.pch.h"
#include "common/time/qp_clock.h"

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

#if defined( QP_VULKAN )
	m_graphicsAPI = qpCreateUnique< qpVulkan >();
#endif

	m_graphicsAPI->Init( m_window.Raw() );

	// initializing to NaN for error checking.
	m_renderCamera.projection = g_mat4NaN;
	m_renderCamera.view = g_mat4NaN;
}

void qpWindowedApp::OnBeginFrame () {
	qpTimePoint currentTime = qpClock::Now();
	m_deltaTime = ( currentTime - m_beginFrameTime );
	m_beginFrameTime = currentTime;
}

void qpWindowedApp::OnEndFrame () {
	QP_ASSERT_RELEASE_MSG( m_renderCamera.projection != g_mat4NaN, "Render camera must be setup before we draw a frame!" );
	QP_ASSERT_RELEASE_MSG( m_renderCamera.view != g_mat4NaN, "Render camera must be setup before we draw a frame!" );
	m_graphicsAPI->DrawFrame( m_renderCamera );
	m_window->OnUpdate();

}

void qpWindowedApp::OnCleanup() {
	m_graphicsAPI.Reset();
	m_window.Reset();
}

#endif 