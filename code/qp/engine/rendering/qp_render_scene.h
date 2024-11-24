#pragma once
#include "qp_render_camera.h"

class qpRenderScene {
public:
	void SetRenderCamera( const renderCamera_t & renderCamera ) { m_renderCamera = renderCamera; }
	const renderCamera_t & GetRenderCamera() const { return m_renderCamera; }
private:
	renderCamera_t m_renderCamera;
};
