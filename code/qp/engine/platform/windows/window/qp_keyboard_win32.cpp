#include "engine.pch.h"

#if !defined( QP_HEADLESS )

#if defined( QP_PLATFORM_WINDOWS )

#include "qp_keyboard_win32.h"

namespace qp {
	constexpr keyboardKeys_t qpFromVirtualKey( const WPARAM key ) {
		switch ( key ) {
			case VK_BACK: return keyboardKeys_t::BACKSPACE;
			case VK_TAB: return keyboardKeys_t::TAB;
			case VK_CLEAR: return keyboardKeys_t::CLEAR;
			case VK_RETURN: return keyboardKeys_t::ENTER;
			case VK_SHIFT: return keyboardKeys_t::SHIFT;
			case VK_CONTROL: return keyboardKeys_t::CTRL;
			case VK_MENU: return keyboardKeys_t::ALT;
			case VK_PAUSE: return keyboardKeys_t::PAUSE;
			case VK_CAPITAL: return keyboardKeys_t::CAPS_LOCK;
			case VK_ESCAPE: return keyboardKeys_t::ESCAPE;
			case VK_SPACE: return keyboardKeys_t::SPACE;
			case VK_PRIOR: return keyboardKeys_t::PAGE_UP;
			case VK_NEXT: return keyboardKeys_t::PAGE_DOWN;
			case VK_END: return keyboardKeys_t::END;
			case VK_HOME: return keyboardKeys_t::HOME;
			case VK_LEFT: return keyboardKeys_t::LEFT;
			case VK_UP: return keyboardKeys_t::UP;
			case VK_RIGHT: return keyboardKeys_t::RIGHT;
			case VK_DOWN: return keyboardKeys_t::DOWN;
			case VK_SELECT: return keyboardKeys_t::SELECT;
			case VK_SNAPSHOT: return keyboardKeys_t::PRINT_SCREEN;
			case VK_INSERT: return keyboardKeys_t::INSERT;
			case VK_DELETE: return keyboardKeys_t::DELETE;
			case VK_HELP: return keyboardKeys_t::HELP;
			case '0': return keyboardKeys_t::N0;
			case '1': return keyboardKeys_t::N1;
			case '2': return keyboardKeys_t::N2;
			case '3': return keyboardKeys_t::N3;
			case '4': return keyboardKeys_t::N4;
			case '5': return keyboardKeys_t::N5;
			case '6': return keyboardKeys_t::N6;
			case '7': return keyboardKeys_t::N7;
			case '8': return keyboardKeys_t::N8;
			case '9': return keyboardKeys_t::N9;
			case 'A': return keyboardKeys_t::A;
			case 'B': return keyboardKeys_t::B;
			case 'C': return keyboardKeys_t::C;
			case 'D': return keyboardKeys_t::D;
			case 'E': return keyboardKeys_t::E;
			case 'F': return keyboardKeys_t::F;
			case 'G': return keyboardKeys_t::G;
			case 'H': return keyboardKeys_t::H;
			case 'I': return keyboardKeys_t::I;
			case 'J': return keyboardKeys_t::J;
			case 'K': return keyboardKeys_t::K;
			case 'L': return keyboardKeys_t::L;
			case 'M': return keyboardKeys_t::M;
			case 'N': return keyboardKeys_t::N;
			case 'O': return keyboardKeys_t::O;
			case 'P': return keyboardKeys_t::P;
			case 'Q': return keyboardKeys_t::Q;
			case 'R': return keyboardKeys_t::R;
			case 'S': return keyboardKeys_t::S;
			case 'T': return keyboardKeys_t::T;
			case 'U': return keyboardKeys_t::U;
			case 'V': return keyboardKeys_t::V;
			case 'W': return keyboardKeys_t::W;
			case 'X': return keyboardKeys_t::X;
			case 'Y': return keyboardKeys_t::Y;
			case 'Z': return keyboardKeys_t::Z;
			case VK_LWIN: return keyboardKeys_t::L_COMMAND;
			case VK_RWIN: return keyboardKeys_t::R_COMMAND;
			case VK_SLEEP: return keyboardKeys_t::SLEEP;
			case VK_NUMPAD0: return keyboardKeys_t::NUMPAD_0;
			case VK_NUMPAD1: return keyboardKeys_t::NUMPAD_1;
			case VK_NUMPAD2: return keyboardKeys_t::NUMPAD_2;
			case VK_NUMPAD3: return keyboardKeys_t::NUMPAD_3;
			case VK_NUMPAD4: return keyboardKeys_t::NUMPAD_4;
			case VK_NUMPAD5: return keyboardKeys_t::NUMPAD_5;
			case VK_NUMPAD6: return keyboardKeys_t::NUMPAD_6;
			case VK_NUMPAD7: return keyboardKeys_t::NUMPAD_7;
			case VK_NUMPAD8: return keyboardKeys_t::NUMPAD_8;
			case VK_NUMPAD9: return keyboardKeys_t::NUMPAD_9;
			case VK_MULTIPLY: return keyboardKeys_t::NUMPAD_MULTIPLY;
			case VK_ADD: return keyboardKeys_t::NUMPAD_ADD;
			case VK_SEPARATOR: return keyboardKeys_t::NUMPAD_SEPARATOR;
			case VK_SUBTRACT: return keyboardKeys_t::NUMPAD_SUBTRACT;
			case VK_DECIMAL: return keyboardKeys_t::NUMPAD_DECIMAL;
			case VK_DIVIDE: return keyboardKeys_t::NUMPAD_DIVIDE;
			case VK_F1: return keyboardKeys_t::F1;
			case VK_F2: return keyboardKeys_t::F2;
			case VK_F3: return keyboardKeys_t::F3;
			case VK_F4: return keyboardKeys_t::F4;
			case VK_F5: return keyboardKeys_t::F5;
			case VK_F6: return keyboardKeys_t::F6;
			case VK_F7: return keyboardKeys_t::F7;
			case VK_F8: return keyboardKeys_t::F8;
			case VK_F9: return keyboardKeys_t::F9;
			case VK_F10: return keyboardKeys_t::F10;
			case VK_F11: return keyboardKeys_t::F11;
			case VK_F12: return keyboardKeys_t::F12;
			case VK_F13: return keyboardKeys_t::F13;
			case VK_F14: return keyboardKeys_t::F14;
			case VK_F15: return keyboardKeys_t::F15;
			case VK_F16: return keyboardKeys_t::F16;
			case VK_F17: return keyboardKeys_t::F17;
			case VK_F18: return keyboardKeys_t::F18;
			case VK_F19: return keyboardKeys_t::F19;
			case VK_F20: return keyboardKeys_t::F20;
			case VK_F21: return keyboardKeys_t::F21;
			case VK_F22: return keyboardKeys_t::F22;
			case VK_F23: return keyboardKeys_t::F23;
			case VK_F24: return keyboardKeys_t::F24;
			case VK_NUMLOCK: return keyboardKeys_t::NUM_LOCK;
			case VK_SCROLL: return keyboardKeys_t::SCROLL_LOCK;
			case VK_LSHIFT: return keyboardKeys_t::L_SHIFT;
			case VK_RSHIFT: return keyboardKeys_t::R_SHIFT;
			case VK_LCONTROL: return keyboardKeys_t::L_CTRL;
			case VK_RCONTROL: return keyboardKeys_t::R_CTRL;
			case VK_LMENU: return keyboardKeys_t::L_ALT;
			case VK_RMENU: return keyboardKeys_t::R_ALT;
			default: return keyboardKeys_t::INVALID;
		}
	}
}

#include "qp/common/platform/windows/qp_windows.h"

namespace qp {
	bool Keyboard_Win32::ProcessWindowEvent( UINT msg, WPARAM wparam, LPARAM lparam ) {
		switch ( msg ) {
			case WM_KEYUP: {
				keyboardKeys_t key = qpFromVirtualKey( wparam );
				if ( key != keyboardKeys_t::INVALID ) {
					m_workingState.ClearBit( key );
					return true;
				}
				return false;
			}
			case WM_KEYDOWN: {
				keyboardKeys_t key = qpFromVirtualKey( wparam );
				if ( key != keyboardKeys_t::INVALID ) {
					m_workingState.SetBit( key );
					return true;
				}
				return false;
			}
			default: {
				return false;
			}
		}
	}
}
#endif

#endif