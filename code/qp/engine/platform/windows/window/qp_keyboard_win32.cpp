#include "engine.pch.h"

#if !defined( QP_HEADLESS )

#if defined( QP_PLATFORM_WINDOWS )

#include "qp_keyboard_win32.h"

namespace qp {
	constexpr keyCode_t MapVKToKeyCode( const WORD vk ) {
		switch ( vk ) {
			case VK_BACK: return keyCode_t::BACKSPACE;
			case VK_TAB: return keyCode_t::TAB;
			case VK_CLEAR: return keyCode_t::CLEAR;
			case VK_RETURN: return keyCode_t::ENTER;
			case VK_PAUSE: return keyCode_t::PAUSE;
			case VK_CAPITAL: return keyCode_t::CAPS_LOCK;
			case VK_ESCAPE: return keyCode_t::ESCAPE;
			case VK_SPACE: return keyCode_t::SPACE;
			case VK_PRIOR: return keyCode_t::PAGE_UP;
			case VK_NEXT: return keyCode_t::PAGE_DOWN;
			case VK_END: return keyCode_t::END;
			case VK_HOME: return keyCode_t::HOME;
			case VK_LEFT: return keyCode_t::LEFT;
			case VK_UP: return keyCode_t::UP;
			case VK_RIGHT: return keyCode_t::RIGHT;
			case VK_DOWN: return keyCode_t::DOWN;
			case VK_SELECT: return keyCode_t::SELECT;
			case VK_SNAPSHOT: return keyCode_t::PRINT_SCREEN;
			case VK_INSERT: return keyCode_t::INSERT;
			case VK_DELETE: return keyCode_t::DELETE;
			case VK_HELP: return keyCode_t::HELP;
			case '0': return keyCode_t::N0;
			case '1': return keyCode_t::N1;
			case '2': return keyCode_t::N2;
			case '3': return keyCode_t::N3;
			case '4': return keyCode_t::N4;
			case '5': return keyCode_t::N5;
			case '6': return keyCode_t::N6;
			case '7': return keyCode_t::N7;
			case '8': return keyCode_t::N8;
			case '9': return keyCode_t::N9;
			case 'A': return keyCode_t::A;
			case 'B': return keyCode_t::B;
			case 'C': return keyCode_t::C;
			case 'D': return keyCode_t::D;
			case 'E': return keyCode_t::E;
			case 'F': return keyCode_t::F;
			case 'G': return keyCode_t::G;
			case 'H': return keyCode_t::H;
			case 'I': return keyCode_t::I;
			case 'J': return keyCode_t::J;
			case 'K': return keyCode_t::K;
			case 'L': return keyCode_t::L;
			case 'M': return keyCode_t::M;
			case 'N': return keyCode_t::N;
			case 'O': return keyCode_t::O;
			case 'P': return keyCode_t::P;
			case 'Q': return keyCode_t::Q;
			case 'R': return keyCode_t::R;
			case 'S': return keyCode_t::S;
			case 'T': return keyCode_t::T;
			case 'U': return keyCode_t::U;
			case 'V': return keyCode_t::V;
			case 'W': return keyCode_t::W;
			case 'X': return keyCode_t::X;
			case 'Y': return keyCode_t::Y;
			case 'Z': return keyCode_t::Z;
			case VK_LWIN: return keyCode_t::L_CMD;
			case VK_RWIN: return keyCode_t::R_CMD;
			case VK_SLEEP: return keyCode_t::SLEEP;
			case VK_NUMPAD0: return keyCode_t::NUMPAD_0;
			case VK_NUMPAD1: return keyCode_t::NUMPAD_1;
			case VK_NUMPAD2: return keyCode_t::NUMPAD_2;
			case VK_NUMPAD3: return keyCode_t::NUMPAD_3;
			case VK_NUMPAD4: return keyCode_t::NUMPAD_4;
			case VK_NUMPAD5: return keyCode_t::NUMPAD_5;
			case VK_NUMPAD6: return keyCode_t::NUMPAD_6;
			case VK_NUMPAD7: return keyCode_t::NUMPAD_7;
			case VK_NUMPAD8: return keyCode_t::NUMPAD_8;
			case VK_NUMPAD9: return keyCode_t::NUMPAD_9;
			case VK_MULTIPLY: return keyCode_t::NUMPAD_MULTIPLY;
			case VK_ADD: return keyCode_t::NUMPAD_ADD;
			case VK_SEPARATOR: return keyCode_t::NUMPAD_SEPARATOR;
			case VK_SUBTRACT: return keyCode_t::NUMPAD_SUBTRACT;
			case VK_DECIMAL: return keyCode_t::NUMPAD_DECIMAL;
			case VK_DIVIDE: return keyCode_t::NUMPAD_DIVIDE;
			case VK_F1: return keyCode_t::F1;
			case VK_F2: return keyCode_t::F2;
			case VK_F3: return keyCode_t::F3;
			case VK_F4: return keyCode_t::F4;
			case VK_F5: return keyCode_t::F5;
			case VK_F6: return keyCode_t::F6;
			case VK_F7: return keyCode_t::F7;
			case VK_F8: return keyCode_t::F8;
			case VK_F9: return keyCode_t::F9;
			case VK_F10: return keyCode_t::F10;
			case VK_F11: return keyCode_t::F11;
			case VK_F12: return keyCode_t::F12;
			case VK_F13: return keyCode_t::F13;
			case VK_F14: return keyCode_t::F14;
			case VK_F15: return keyCode_t::F15;
			case VK_F16: return keyCode_t::F16;
			case VK_F17: return keyCode_t::F17;
			case VK_F18: return keyCode_t::F18;
			case VK_F19: return keyCode_t::F19;
			case VK_F20: return keyCode_t::F20;
			case VK_F21: return keyCode_t::F21;
			case VK_F22: return keyCode_t::F22;
			case VK_F23: return keyCode_t::F23;
			case VK_F24: return keyCode_t::F24;
			case VK_NUMLOCK: return keyCode_t::NUM_LOCK;
			case VK_SCROLL: return keyCode_t::SCROLL_LOCK;
			case VK_LSHIFT: return keyCode_t::L_SHIFT;
			case VK_RSHIFT: return keyCode_t::R_SHIFT;
			case VK_LCONTROL: return keyCode_t::L_CTRL;
			case VK_RCONTROL: return keyCode_t::R_CTRL;
			case VK_LMENU: return keyCode_t::L_ALT;
			case VK_RMENU: return keyCode_t::R_ALT;
			case VK_SHIFT: { 
				QP_ASSERT_ALWAYS( "VK_SHIFT should've been converted to LEFT / RIGHT hand variant" ); 
				return keyCode_t::INVALID; 
			}
			case VK_CONTROL: { 
				QP_ASSERT_ALWAYS( "VK_CONTROL should've been converted to LEFT / RIGHT hand variant" ); 
				return keyCode_t::INVALID; 
			}
			case VK_MENU: { 
				QP_ASSERT_ALWAYS( "VK_MENU should've been converted to LEFT / RIGHT hand variant" ); 
				return keyCode_t::INVALID; 
			}
			default: 
				return keyCode_t::INVALID; // we could map these to a reserved range between our max value + 256
		}
	}
}

#include "qp/common/platform/windows/qp_windows.h"

namespace qp {
	constexpr WORD GetScancode( const LPARAM lparam ) {
		const WORD keyFlags = ( lparam >> 16 ) & 0xffff;
		return ( keyFlags & 0xff ) | ( ( ( keyFlags & KF_EXTENDED ) != 0 ) ? ( 0xE0 << 8 ) : 0 );
	}
	static WORD GetScancodeAsVK( const LPARAM lparam ) {
		const UINT scancode = GetScancode( lparam );
		const UINT vkFromScancode = MapVirtualKey( scancode, MAPVK_VSC_TO_VK_EX );
		QP_ASSERT( vkFromScancode != 0 );
		QP_ASSERT_RELEASE( ( vkFromScancode & 0xffff ) == vkFromScancode );
		return vkFromScancode & 0xffff;
	}
	
	bool Keyboard_Win32::ProcessWindowEvent( UINT msg, WPARAM wparam, LPARAM lparam ) {
		switch ( msg ) {
			case WM_KEYUP: {
				keyCode_t key = MapVKToKeyCode( GetScancodeAsVK( lparam ) );
				return HandleEvent( key, false );
			}
			case WM_KEYDOWN: {
				keyCode_t key = MapVKToKeyCode( GetScancodeAsVK( lparam ) );
				return HandleEvent( key, true );
			}
		}

		return false;
	}
}
#endif

#endif