#pragma once

#if !defined( QP_HEADLESS )

#include "qp_app.h"
#include "qp/common/core/qp_unique_ptr.h"
#include "qp/engine/window/qp_window_structs.h"
#include "engine/rendering/qp_graphics_api.h"

#include "qp/engine/window/qp_window.h"

namespace qp {
	class RenderScene;
	class Window;
	class WindowedApp : public App {
	public:
		explicit WindowedApp( const windowProperties_t & windowProperties );
		virtual void OnInit() override;
		virtual void OnUpdate() override {};
		virtual void OnCleanup() override;
	protected:
		UniquePtr< Window > m_window;
		UniquePtr< GraphicsAPI > m_graphicsAPI;
		UniquePtr< RenderScene > m_renderScene;
		windowProperties_t m_windowProperties;
		TimePoint m_beginFrameTime = g_timePointInvalid;
		TimePoint m_deltaTime = g_timePointInvalid;
	private:
		virtual void OnBeginFrame() override;
		virtual void OnEndFrame() override;
	};
}

#endif