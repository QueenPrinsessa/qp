#pragma once

#include "engine/core/qp_app.h"

namespace qp {
	class HeadlessApp : public App {
	public:
		HeadlessApp() = default;
		virtual void OnInit() override;
		virtual void OnBeginUpdate() override;
		virtual void OnUpdate() override;
		virtual void OnEndUpdate() override;
		virtual void OnCleanup() override;
	private:
	};
}