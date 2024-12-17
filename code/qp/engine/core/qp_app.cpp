#include "engine.pch.h"
#include "qp_app.h"

namespace qp {
	App::App() {
	}

	App::~App() {
	}
	
	void App::RequestShutdown() {
		m_isRunning = false;
	}

	void App::Run() {
		m_isRunning = true;

		QP_ASSERT_RELEASE( thread_util::InitializeMainThread() );

		OnInit();

		while ( m_isRunning ) {
			OnBeginUpdate();
			OnUpdate();
			OnEndUpdate();
		}

		OnCleanup();
	}
}