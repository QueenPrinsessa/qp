#pragma once
#if defined( QP_PLATFORM_WINDOWS )

#include "qp/common/platform/windows/qp_windows.h"
#include "qp/engine/window/qp_window.h"
#include "qp_mouse_windows.h"
#include "qp_keyboard_windows.h"

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
	virtual windowMode_t GetWindowMode() const override { return m_windowMode; }

	void SetWindowMode( const windowMode_t windowMode ) { m_windowMode = windowMode; }

	virtual const qpMouse & GetMouse() const override { return m_mouse; }
	virtual const qpKeyboard & GetKeyboard() const override { return m_keyboard; }
private:
	void Init( const qpWindowProperties_t & properties );
	void ApplyWindowMode( const windowMode_t windowMode );

	qpWindowsKeyboard m_keyboard;
	qpWindowsMouse m_mouse;
	WINDOWPLACEMENT m_lastWindowPlacement = { sizeof( WINDOWPLACEMENT ) };
	HWND m_handle = NULL;
	int m_width = 1280;
	int m_height = 720;
	windowMode_t m_windowMode = windowMode_t::WINDOWED;

	static LRESULT CALLBACK WndProc( _In_ HWND handle, _In_ UINT msg, _In_ WPARAM wparam, _In_ LPARAM lparam );
};

#endif
