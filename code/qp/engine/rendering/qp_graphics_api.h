#pragma once

#if !defined( QP_HEADLESS )

struct renderCamera_t;
class qpWindow;

class qpGraphicsAPI {
public:
	qpGraphicsAPI() = default;
	virtual ~qpGraphicsAPI() = default;

	virtual void Init( const qpWindow * window ) = 0;
	virtual void DrawFrame( const renderCamera_t & renderCamera ) = 0;
	virtual void Cleanup() = 0;

	void RequestFramebufferResize();
protected:
	bool m_frameBufferResized = false;
};

#endif