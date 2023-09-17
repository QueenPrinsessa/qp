#pragma once
#ifdef QP_PLATFORM_WINDOWS

#include "qp/common/platform/windows/qp_windows.h"
#include "qp/engine/window/qp_window.h"

struct qpWindowPropertiesWindows_t {
	HINSTANCE instanceHandle = NULL;
};

class qpWindowsWindow : public qpWindow {
public:
	qpWindowsWindow( const qpWindowProperties_t & properties );
	virtual ~qpWindowsWindow() override;

	virtual void OnUpdate() override;

	virtual int GetWidth() override { return m_width; }
	virtual int GetHeight() override { return m_height; }
	virtual void * GetHandle() const override { return m_handle; }
	virtual qpWindowMode_t GetWindowMode() const override { return m_windowMode; }

	void SetWindowMode( const qpWindowMode_t windowMode ) { m_windowMode = windowMode; }
	
private:
	void Init( const qpWindowProperties_t & properties );
	void ApplyWindowMode( const qpWindowMode_t windowMode );

	int m_width = 1280;
	int m_height = 720;
	qpWindowMode_t m_windowMode = qpWindowMode_t::WINDOWED;
	HWND m_handle = NULL;
	WINDOWPLACEMENT m_lastWindowPlacement = { sizeof( WINDOWPLACEMENT ) };

	static LRESULT CALLBACK WndProc( _In_ HWND handle, _In_ UINT msg, _In_ WPARAM wparam, _In_ LPARAM lparam );
};

#endif
