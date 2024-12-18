#include "engine.pch.h"

#if defined( QP_PLATFORM_WINDOWS )
#if !defined( QP_HEADLESS )

#include "qp_mouse_cursor_win32.h"
#include "common/platform/windows/qp_windows.h"
namespace qp {
	void MouseCursor_Win32::Update() {
		
		const bool regainedFocus = m_focused && !m_lastFocused;
		const bool lostFocus = !m_focused && m_lastFocused;
		m_lastFocused = m_focused;

		m_nextDelta = m_nextPosition - m_position;

		if ( lostFocus || regainedFocus ) {
			m_nextDelta.Zero();
		}

		m_position = m_nextPosition;
		m_delta = m_nextDelta;

		m_nextPosition = m_position;
		m_nextDelta.Zero();

		if ( IsVisible() ) {
			while ( ::ShowCursor( TRUE ) < 0 ) {}
		} else {
			while ( ::ShowCursor( FALSE ) >= 0 ) {}
		}
	}

	bool MouseCursor_Win32::ProcessWindowEvent( HWND handle, UINT msg, WPARAM wparam, LPARAM lparam ) {
		switch ( msg ) {
			case WM_ACTIVATE: {
				m_focused = LOWORD( wparam ) != WA_INACTIVE;
				return false; // pass along
			}
			case WM_MOUSEMOVE: {
				POINT point { GET_X_LPARAM( lparam ), GET_Y_LPARAM( lparam ) };
				m_nextPosition.x = static_cast< float >( point.x );
				m_nextPosition.y = static_cast< float >( point.y );
				return true;
			}
		}
		return false;
	}
}
#endif
#endif