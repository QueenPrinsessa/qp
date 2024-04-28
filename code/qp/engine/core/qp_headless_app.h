#pragma once

#include "engine/core/qp_app.h"
#include "qp/common/core/qp_unique_ptr.h"
#include "qp/engine/window/qp_window_structs.h"
#include "qp/engine/rendering/qp_vulkan.h"

class qpHeadlessApp : public qpApp {
public:
	qpHeadlessApp() = default;
	virtual void OnInit() override;
	virtual void OnUpdate() override;
	virtual void OnCleanup() override;
private:
};