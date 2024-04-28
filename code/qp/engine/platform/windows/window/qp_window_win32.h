#pragma once

#if !defined( QP_HEADLESS )

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/engine/window/qp_window.h"
#include "qp/common/platform/windows/qp_types_win32.h"

struct windowPropertiesWindows_t {
	HINSTANCE instanceHandle = NULL;
};

struct windowWin32Data_t;
class qpWindow_Win32 : public qpWindow {
public:
	qpWindow_Win32( const windowProperties_t & properties );
	virtual ~qpWindow_Win32() override;

	virtual void OnUpdate() override;

	virtual int GetWidth() override { return m_width; }
	virtual int GetHeight() override { return m_height; }
	virtual void * GetHandle() const override { return m_handle; }
	virtual windowMode_t GetWindowMode() const override { return m_windowMode; }

	void SetWindowMode( const windowMode_t windowMode ) { m_windowMode = windowMode; }

	virtual const qpMouse & GetMouse() const override;
	virtual const qpKeyboard & GetKeyboard() const override;
private:
	void Init( const windowProperties_t & properties );
	void ApplyWindowMode( const windowMode_t windowMode );

	qpUniquePtr< windowWin32Data_t > m_data;
	HWND m_handle = NULL;
	int m_width = 1280;
	int m_height = 720;
	windowMode_t m_windowMode = windowMode_t::WINDOWED;
	static LRESULT CALLBACK WndProc( _In_ HWND handle, _In_ UINT msg, _In_ WPARAM wparam, _In_ LPARAM lparam );
};

#endif

#endif