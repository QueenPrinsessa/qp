#pragma once

#if defined( QP_PLATFORM_WINDOWS )
#if !defined( QP_HEADLESS )

#include "qp/engine/input/qp_cursor.h"
#include "qp/common/platform/windows/qp_types_win32.h"
#include "common/containers/qp_list.h"
#include <mutex>

namespace qp {
	class MouseCursor_Win32 : public Cursor {
		friend class Mouse_Win32;
	public:
		virtual void Update() override;
		
	private:
		bool ProcessWindowEvent( HWND handle, UINT msg, WPARAM wparam, LPARAM lparam ); // returns true if handled.
		List< byte > m_rawInputBuffer;
		bool m_focused = false;
		bool m_lastFocused = false;
	};
}
#endif
#endif