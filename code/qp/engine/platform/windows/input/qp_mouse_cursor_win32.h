#pragma once

#if defined( QP_PLATFORM_WINDOWS )
#if !defined( QP_HEADLESS )

#include "qp/engine/input/qp_cursor.h"
#include "qp/common/platform/windows/qp_types_win32.h"
#include <mutex>

namespace qp {
	class MouseCursor_Win32 : public Cursor {
		friend class Mouse_Win32;
	public:
		MouseCursor_Win32( HWND windowHandle );
		virtual void Update() override;
		
	private:
		bool ProcessWindowEvent( UINT msg, WPARAM wparam, LPARAM lparam ); // returns true if handled.
		std::mutex m_pollMutex;
		HWND m_windowHandle = NULL;
		bool m_isFocused = false;
		// settings
		bool m_captureCursor = true;
		bool m_clipCursor = false;
		bool m_lockToCenter = true;
		// state
		bool m_capturedCursor = false;
		bool m_clippedCursor = false;
	};
}
#endif
#endif