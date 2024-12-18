#include "engine.pch.h"

#if !defined( QP_HEADLESS )

#if defined( QP_PLATFORM_WINDOWS )

#include "qp_mouse_win32.h"

#include "qp/common/platform/windows/qp_windows.h"
#include <hidusage.h>

namespace qp {
	bool Mouse_Win32::InitializeRawInput( HWND handle ) {
		if ( !m_rawInputInitialized ) {
			RAWINPUTDEVICE RID[ 1 ];
			RID[ 0 ].usUsagePage = HID_USAGE_PAGE_GENERIC;
			RID[ 0 ].usUsage = HID_USAGE_GENERIC_MOUSE;
			RID[ 0 ].dwFlags = RIDEV_INPUTSINK;
			RID[ 0 ].hwndTarget = handle;

			if ( !RegisterRawInputDevices( &RID[ 0 ], 1, sizeof( RID[ 0 ] ) ) ) {
				m_rawInputInitialized = true;
				QP_ASSERT_ALWAYS( "Failed to register raw input device!" );
				return true;
			}
		}
		return false;
	}

	bool Mouse_Win32::ProcessWindowEvent( HWND handle, UINT msg, WPARAM wparam, LPARAM lparam ) {
		if ( m_mouseCursor.ProcessWindowEvent( handle, msg, wparam, lparam ) ) {
			return true;
		}
	
		 switch ( msg ) {
		 	case WM_ACTIVATE: {
				m_focused = LOWORD( wparam ) != WA_INACTIVE;
				return false; // pass along
			}
		 	case WM_LBUTTONUP: {
				return HandleEvent( mouseButton_t::LBUTTON, false );
			}
		 	case WM_LBUTTONDOWN: {
				return HandleEvent( mouseButton_t::LBUTTON, true );
			}
			case WM_RBUTTONUP: {
				return HandleEvent( mouseButton_t::RBUTTON, false );
			}
		 	case WM_RBUTTONDOWN: {
				return HandleEvent( mouseButton_t::RBUTTON, true );

			}
			case WM_MBUTTONUP: {
				return HandleEvent( mouseButton_t::MBUTTON, false );
			}
			case WM_MBUTTONDOWN: {
				return HandleEvent( mouseButton_t::MBUTTON, true );
			}
		 	case WM_XBUTTONUP: {
				WORD xbutton = GET_XBUTTON_WPARAM( wparam );
				mouseButton_t button = mouseButton_t::INVALID;
				if ( xbutton == XBUTTON1 ) {
					button = mouseButton_t::MOUSE4;
				} else if ( xbutton == XBUTTON2 ) {
					button = mouseButton_t::MOUSE5;
				}
				return HandleEvent( button, false );
			}
			case WM_XBUTTONDOWN: {
				WORD xbutton = GET_XBUTTON_WPARAM( wparam );
				mouseButton_t button = mouseButton_t::INVALID;
				if ( xbutton == XBUTTON1 ) {
					button = mouseButton_t::MOUSE4;
				} else if ( xbutton == XBUTTON2 ) {
					button = mouseButton_t::MOUSE5;
				}
				return HandleEvent( button, false );
			}
			case WM_MOUSEWHEEL: {
				return HandleScrollEvent( 0, static_cast< float >( GET_WHEEL_DELTA_WPARAM( wparam ) ) / static_cast< float >( WHEEL_DELTA ) );
			}
			case WM_MOUSEHWHEEL: {
				return HandleScrollEvent( static_cast< float >( GET_WHEEL_DELTA_WPARAM( wparam ) ) / static_cast< float >( WHEEL_DELTA ), 0 );
			}
		 }
	
		 return false;
	}
}
#endif

#endif