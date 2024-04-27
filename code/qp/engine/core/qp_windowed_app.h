#pragma once
#include "qp_app.h"
#include "qp/common/core/qp_unique_ptr.h"
#include "qp/engine/window/qp_window_structs.h"

class qpVulkan;
class qpWindow;
class qpWindowedApp : public qpApp {
public:
	explicit qpWindowedApp( const windowProperties_t & windowProperties );
	virtual void OnInit() override;
	virtual void OnUpdate() override;
	virtual void OnCleanup() override;
private:
	qpUniquePtr< qpWindow > m_window;
	qpUniquePtr< qpVulkan > m_vulkan;
	windowProperties_t m_windowProperties;
};
