#include "engine.pch.h"

#if !defined( QP_HEADLESS )

#if defined( QP_PLATFORM_WINDOWS )

#include "qp_mouse_win32.h"

#include "qp/common/platform/windows/qp_windows.h"

qpMouse_Win32::qpMouse_Win32 ( HWND windowHandle ) 
	: m_mouseCursor( windowHandle ){
}

bool qpMouse_Win32::ProcessWindowEvent( UINT msg, WPARAM wparam, LPARAM lparam ) {
	if ( m_mouseCursor.ProcessWindowEvent( msg, wparam, lparam ) ) {
		return true;
	}

	 switch ( msg ) {
	 	case WM_LBUTTONUP: {
			m_workingState.ClearBit( mouseButton_t::LBUTTON );
			return true;
		}
	 	case WM_LBUTTONDOWN: {
			m_workingState.SetBit( mouseButton_t::LBUTTON );
			return true;
		}
		case WM_RBUTTONUP: {
			m_workingState.ClearBit( mouseButton_t::RBUTTON );
			return true;
		}
	 	case WM_RBUTTONDOWN: {
			m_workingState.SetBit( mouseButton_t::RBUTTON );
			return true;
		}
		case WM_MBUTTONUP: {
			m_workingState.ClearBit( mouseButton_t::MBUTTON );
			return true;
		}
		case WM_MBUTTONDOWN: {
			m_workingState.SetBit( mouseButton_t::MBUTTON );
			return true;
		}
	 	case WM_XBUTTONUP: {
			WORD xbutton = GET_XBUTTON_WPARAM( wparam );
			if ( xbutton == XBUTTON1 ) {
				m_workingState.ClearBit( mouseButton_t::MOUSE4 );
			} else if ( xbutton == XBUTTON2 ) {
				m_workingState.ClearBit( mouseButton_t::MOUSE5 );
			}
			return true;
		}
		case WM_XBUTTONDOWN: {
			WORD xbutton = GET_XBUTTON_WPARAM( wparam );
			if ( xbutton == XBUTTON1 ) {
				m_workingState.SetBit( mouseButton_t::MOUSE4 );
			} else if ( xbutton == XBUTTON2 ) {
				m_workingState.SetBit( mouseButton_t::MOUSE5 );
			}
			return true;
		}
		case WM_MOUSEWHEEL: {
			float wheelDelta = static_cast< float >( GET_WHEEL_DELTA_WPARAM( wparam ) ) / static_cast< float >( WHEEL_DELTA );
			m_workingScrollValue.y += wheelDelta;
			return true;
		}

		case WM_MOUSEHWHEEL: {
			float wheelDelta = static_cast< float >( GET_WHEEL_DELTA_WPARAM( wparam ) ) / static_cast< float >( WHEEL_DELTA );
			m_workingScrollValue.x += wheelDelta;
			return true;
		}
	 }

	 return false;
}
#endif

#endif