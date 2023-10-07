#pragma once
#include "qp_app.h"
#include "qp/common/core/qp_unique_ptr.h"
#include "qp/engine/rendering/qp_vulkan.h"
#include "qp/engine/window/qp_window.h"

class qpWindowedApp : public qpApp {
public:
	explicit qpWindowedApp( const qpWindowProperties_t & windowProperties );
	virtual void OnInit() override;
	virtual void OnUpdate() override;
	virtual void OnCleanup() override;
private:
	qpUniquePtr< qpWindow > m_window;
	qpUniquePtr< qpVulkan > m_vulkan;
	qpWindowProperties_t m_windowProperties;
};
