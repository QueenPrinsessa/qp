#pragma once

namespace qp {
	class App {
	public:
		App();
		virtual ~App();

		void Run();
		void RequestShutdown();

		bool IsRunning() const { return m_isRunning; }

		virtual void OnInit() = 0;
		virtual void OnBeginFrame() = 0;
		virtual void OnUpdate() = 0;
		virtual void OnEndFrame() = 0;
		virtual void OnCleanup() = 0;

	private:
		bool m_isRunning = false;
	};
}