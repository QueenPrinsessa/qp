#include "engine.pch.h"
#include "qp_graphics_api.h"

#if !defined( QP_HEADLESS )
namespace qp {
	void GraphicsAPI::RequestFramebufferResize() {
		m_frameBufferResized = true;
	}
}
#endif