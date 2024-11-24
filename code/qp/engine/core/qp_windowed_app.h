#pragma once

#if !defined( QP_HEADLESS )

#include "qp_app.h"
#include "qp/common/core/qp_unique_ptr.h"
#include "qp/engine/window/qp_window_structs.h"
#include "engine/rendering/qp_graphics_api.h"

#include "qp/engine/window/qp_window.h"

class qpRenderScene;
class qpWindow;
class qpWindowedApp : public qpApp {
public:
	explicit qpWindowedApp( const windowProperties_t & windowProperties );
	virtual void OnInit() override;
	virtual void OnUpdate() override {};
	virtual void OnCleanup() override;
protected:
	qpUniquePtr< qpWindow > m_window;
	qpUniquePtr< qpGraphicsAPI > m_graphicsAPI;
	qpUniquePtr< qpRenderScene > m_renderScene;
	windowProperties_t m_windowProperties;
	qpTimePoint m_beginFrameTime = g_timePointInvalid;
	qpTimePoint m_deltaTime = g_timePointInvalid;
private:
	virtual void OnBeginFrame() override;
	virtual void OnEndFrame() override;
};

#endif