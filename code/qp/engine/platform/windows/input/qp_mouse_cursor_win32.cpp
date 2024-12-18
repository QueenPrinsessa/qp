#include "engine.pch.h"

#if defined( QP_PLATFORM_WINDOWS )
#if !defined( QP_HEADLESS )

#include "qp_mouse_cursor_win32.h"
#include "common/platform/windows/qp_windows.h"

#include <hidusage.h>

namespace qp {
	void MouseCursor_Win32::Update() {
		
		const bool regainedFocus = m_focused && !m_lastFocused;
		const bool lostFocus = !m_focused && m_lastFocused;
		m_lastFocused = m_focused;

		// only needed for non-relative mouse movement
		// m_nextDelta = m_nextPosition - m_position;

		if ( lostFocus || regainedFocus ) {
			m_nextDelta.Zero();
		}

		m_position = m_nextPosition;
		if ( m_focused ) {
			m_delta = m_nextDelta;
		}

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
			case WM_INPUT: {
				m_rawInputBuffer.Clear();

				UINT errRetVal = ( UINT )-1;
				UINT size = 0;
				if ( GetRawInputData( reinterpret_cast< HRAWINPUT >( lparam ), RID_INPUT, NULL, &size, sizeof( RAWINPUTHEADER ) ) == errRetVal ) {
					return false;
				}
				m_rawInputBuffer.Resize( size );
				if ( GetRawInputData( reinterpret_cast< HRAWINPUT >( lparam ), RID_INPUT, m_rawInputBuffer.Data(), &size, sizeof( RAWINPUTHEADER ) ) != size ) {
					return false;
				}

				qp::debug::Printf( "SIZE: %u\n", m_rawInputBuffer.Length() );

				RAWINPUT * rawInput = reinterpret_cast< RAWINPUT * >( m_rawInputBuffer.Data() );
				RAWMOUSE rawMouse = rawInput->data.mouse;
				if ( rawInput->header.dwType == RIM_TYPEMOUSE /*&& ( rawMouse.lLastX != 0 ) && ( rawMouse.lLastY != 0 )*/ ) {
					m_nextDelta.x += static_cast< float >( rawMouse.lLastX );
					m_nextDelta.y += static_cast< float >( rawMouse.lLastY );

					qp::debug::Printf( "RAWMOUSE: x:%d y:%d\n", rawMouse.lLastX, rawMouse.lLastY );
				}

				return true;
			}
		}
		return false;
	}
}
#endif
#endif