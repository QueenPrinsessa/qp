#pragma once
#include "common/threads/qp_thread_util.h"

namespace qp {
	class App {
	public:
		App();
		virtual ~App();

		void Run();
		void RequestShutdown();

		bool IsRunning() const { return m_isRunning; }

		virtual void OnInit() = 0;
		virtual void OnBeginUpdate() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnEndUpdate() = 0;
		virtual void OnCleanup() = 0;

	private:
		bool m_isRunning = false;
	};
}