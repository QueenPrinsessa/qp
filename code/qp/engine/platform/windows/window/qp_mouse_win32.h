#pragma once

#if !defined( QP_HEADLESS )

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/engine/window/qp_mouse.h"
#include "engine/platform/windows/input/qp_mouse_cursor_win32.h"
#include "qp/common/platform/windows/qp_types_win32.h"

class qpMouse_Win32 : public qpMouse {
	friend class qpWindow_Win32;
public:
	qpMouse_Win32( HWND windowHandle );
	virtual ~qpMouse_Win32() override = default;

	virtual const qpCursor & GetMouseCursor() const override { return m_mouseCursor; }
private:
	bool ProcessWindowEvent( UINT msg, WPARAM wparam, LPARAM lparam ); // returns true if handled.
	qpMouseCursor_Win32 m_mouseCursor;
};

#endif

#endif