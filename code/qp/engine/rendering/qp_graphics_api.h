#pragma once

#if !defined( QP_HEADLESS )

class qpGraphicsAPI {
public:
	qpGraphicsAPI() = default;
	virtual ~qpGraphicsAPI() = default;

	virtual void Init( void * windowHandle ) = 0;
	virtual void DrawFrame() = 0;
	virtual void Cleanup() = 0;

	void RequestFramebufferResize();
protected:
	bool m_frameBufferResized = false;
};

#endif