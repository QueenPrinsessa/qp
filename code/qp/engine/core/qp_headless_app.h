#pragma once

#include "engine/core/qp_app.h"
#include "qp/common/core/qp_unique_ptr.h"
#include "qp/engine/window/qp_window_structs.h"
#include "qp/engine/rendering/qp_vulkan.h"

namespace qp {
	class HeadlessApp : public App {
	public:
		HeadlessApp() = default;
		virtual void OnInit() override;
		virtual void OnBeginFrame() override;
		virtual void OnUpdate() override;
		virtual void OnEndFrame() override;
		virtual void OnCleanup() override;
	private:
	};
}