#include "engine.pch.h"
#include "qp_mouse_cursor_win32.h"

#include "common/platform/windows/qp_windows.h"

qpMouseCursor_Win32::qpMouseCursor_Win32 ( HWND windowHandle ) 
	: m_windowHandle( windowHandle ) {
}

void qpMouseCursor_Win32::Update() {
	const bool isFocused = ( GetFocus() == m_windowHandle );

	qpVec2i nextPosition = m_position;

	// only control visibility when the window is focused
	if ( isFocused ) {
		POINT cursorPos;
		GetCursorPos( &cursorPos );
		ScreenToClient( m_windowHandle, &cursorPos );
		nextPosition = { cursorPos.x, cursorPos.y };

		if ( m_lockToCenter ) {
			RECT rect;
			GetClientRect( m_windowHandle, &rect );
			POINT center { ( rect.right - rect.left ) / 2, ( rect.bottom - rect.top ) / 2 };

			m_previousPosition = { center.x, center.y };

			ClientToScreen( m_windowHandle, &center );
			SetCursorPos( center.x, center.y );
			
			m_positionDelta = nextPosition - m_previousPosition;
			m_position = m_previousPosition;

			while ( IsVisible() ) {
				Hide();
			}
		}

		if ( IsVisible() ) {
			while ( ::ShowCursor( TRUE ) < 0 ) {}
		} else {
			while ( ::ShowCursor( FALSE ) >= 0 ) {}
		}
	}
	
	if ( !m_lockToCenter || !isFocused ) {
		m_previousPosition = m_position;
		m_position = nextPosition;
		m_positionDelta = m_position - m_previousPosition;
	}

	m_positionDelta = -m_positionDelta;
	
	// just regained focus, reset delta so we don't get a large jump
	const bool swallowPositionDelta = !m_isFocused && isFocused;
	if ( swallowPositionDelta ) {
		m_positionDelta = { 0, 0 };
	}
	
	m_isFocused = isFocused;
	// todo: get the raw delta using rawinput.
}

bool qpMouseCursor_Win32::ProcessWindowEvent( UINT msg, WPARAM wparam, LPARAM lparam ) {

	switch ( msg ) {
		case WM_ACTIVATE: {
			const bool activated = LOWORD( wparam ) != WA_INACTIVE;
			if ( activated ) {
				if ( m_captureCursor ) {
					SetCapture( m_windowHandle );
					m_capturedCursor = true;
				}
				if ( m_clipCursor ) {
					RECT rect;
					GetClientRect( m_windowHandle, &rect );
					ClientToScreen( m_windowHandle, reinterpret_cast< POINT * >( &rect ) );
					ClientToScreen( m_windowHandle, reinterpret_cast< POINT * >( &rect ) + 1 );
					ClipCursor( &rect );
					m_clippedCursor = true;
				}
			} else {
				if ( m_capturedCursor ) {
					ReleaseCapture();
					m_capturedCursor = false;
				}
				if ( m_clippedCursor ) {
					ClipCursor( NULL );
				}
			}
			return true;
		}
	}
	return false;
}
