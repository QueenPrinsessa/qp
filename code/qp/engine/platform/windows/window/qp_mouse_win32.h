#pragma once

#if !defined( QP_HEADLESS )

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/engine/window/qp_mouse.h"
#include "engine/platform/windows/input/qp_mouse_cursor_win32.h"
#include "qp/common/platform/windows/qp_types_win32.h"
namespace qp {
	class Mouse_Win32 : public Mouse {
		friend class Window_Win32;
	public:
		virtual ~Mouse_Win32() override = default;

		virtual const Cursor & GetMouseCursor() const override { return m_mouseCursor; }
		virtual Cursor & GetMouseCursor() override { return m_mouseCursor; }
	private:
		bool InitializeRawInput( HWND handle );
		bool ProcessWindowEvent( HWND handle, UINT msg, WPARAM wparam, LPARAM lparam ); // returns true if handled.
		MouseCursor_Win32 m_mouseCursor;
		bool m_rawInputInitialized = false;
	};
}
#endif

#endif