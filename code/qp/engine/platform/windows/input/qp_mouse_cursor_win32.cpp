#include "engine.pch.h"

#if defined( QP_PLATFORM_WINDOWS )
#if !defined( QP_HEADLESS )

#include "qp_mouse_cursor_win32.h"
#include "common/platform/windows/qp_windows.h"
namespace qp {
	void MouseCursor_Win32::Update() {
		m_nextPosition = m_position;

		Vec2 nextPosition = m_nextPosition;

		// hack: use focus as window...
		HWND focus = GetFocus();
		bool isFocused = focus != NULL;
		if ( isFocused ) {
			m_windowHandle = focus;
			POINT cursorPos;
			GetCursorPos( &cursorPos );
			ScreenToClient( m_windowHandle, &cursorPos );
			nextPosition = { static_cast< float >( cursorPos.x ), static_cast< float >( cursorPos.y ) };

			// todo: get the raw delta using rawinput.
			if ( m_lockToCenter ) {
				RECT rect;
				GetClientRect( m_windowHandle, &rect );
				POINT center { ( rect.right - rect.left ) / 2, ( rect.bottom - rect.top ) / 2 };

				m_previousPosition = { static_cast< float >( center.x ), static_cast< float >( center.y ) };

				ClientToScreen( m_windowHandle, &center );
				SetCursorPos( center.x, center.y );

				m_positionDelta = nextPosition - m_previousPosition;
				m_position = m_previousPosition;

				ForceHide();
			}
		} else {
			ForceShow();
		}

		if ( IsVisible() ) {
			while ( ::ShowCursor( TRUE ) < 0 ) {}
		} else {
			while ( ::ShowCursor( FALSE ) >= 0 ) {}
		}

		if ( !m_lockToCenter || !isFocused ) {
			m_previousPosition = m_position;
			m_position = nextPosition;
			m_positionDelta = m_position - m_previousPosition;
		}

		// just regained focus, reset delta so we don't get a large jump
		const bool swallowPositionDelta = !m_isFocused && isFocused;
		if ( swallowPositionDelta ) {
			m_positionDelta = { 0, 0 };
		}

		m_isFocused = isFocused;
	}

	bool MouseCursor_Win32::ProcessWindowEvent( HWND handle, UINT msg, WPARAM wparam, LPARAM lparam ) {
		//switch ( msg ) {

		//}
		return false;
	}
}
#endif
#endif