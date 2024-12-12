#include "engine.pch.h"
#include "qp_graphics_api.h"

namespace qp {
	void GraphicsAPI::RequestFramebufferResize() {
		m_frameBufferResized = true;
	}
}