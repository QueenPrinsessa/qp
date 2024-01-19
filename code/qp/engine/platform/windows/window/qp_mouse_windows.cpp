#include "engine.pch.h"

#if defined( QP_PLATFORM_WINDOWS )

#include "qp_mouse_windows.h"

bool qpWindowsMouse::ProcessWindowEvent( UINT msg, WPARAM wparam, LPARAM lparam ) {
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
	 	default: {
			return false;
	 	}
	 }
}
#endif