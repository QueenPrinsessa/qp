#pragma once

#if !defined( QP_HEADLESS )

namespace qp {
	struct renderCamera_t;
	class Window;

	class GraphicsAPI {
	public:
		GraphicsAPI() = default;
		virtual ~GraphicsAPI() = default;

		virtual void Init( const Window * window ) = 0;
		virtual void DrawFrame( const renderCamera_t & renderCamera ) = 0;
		virtual void Cleanup() = 0;

		void RequestFramebufferResize();
	protected:
		bool m_frameBufferResized = false;
	};
}
#endif