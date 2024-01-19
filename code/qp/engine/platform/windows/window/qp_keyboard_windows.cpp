#include "engine.pch.h"

#if defined( QP_PLATFORM_WINDOWS )

#include "qp_keyboard_windows.h"

static keyboardKeys_t FromVirtualKey( const WPARAM key ) {
	switch ( key ) {
		case VK_BACK: return keyboardKeys_t::KEY_BACKSPACE;
		case VK_TAB: return keyboardKeys_t::KEY_TAB;
		case VK_CLEAR: return keyboardKeys_t::KEY_CLEAR;
		case VK_RETURN: return keyboardKeys_t::KEY_ENTER;
		case VK_SHIFT: return keyboardKeys_t::KEY_SHIFT;
		case VK_CONTROL: return keyboardKeys_t::KEY_CTRL;
		case VK_MENU: return keyboardKeys_t::KEY_ALT;
		case VK_PAUSE: return keyboardKeys_t::KEY_PAUSE;
		case VK_CAPITAL: return keyboardKeys_t::KEY_CAPS_LOCK;
		case VK_ESCAPE: return keyboardKeys_t::KEY_ESCAPE;
		case VK_SPACE: return keyboardKeys_t::KEY_SPACE;
		case VK_PRIOR: return keyboardKeys_t::KEY_PAGE_UP;
		case VK_NEXT: return keyboardKeys_t::KEY_PAGE_DOWN;
		case VK_END: return keyboardKeys_t::KEY_END;
		case VK_HOME: return keyboardKeys_t::KEY_HOME;
		case VK_LEFT: return keyboardKeys_t::KEY_LEFT;
		case VK_UP: return keyboardKeys_t::KEY_UP;
		case VK_RIGHT: return keyboardKeys_t::KEY_RIGHT;
		case VK_DOWN: return keyboardKeys_t::KEY_DOWN;
		case VK_SELECT: return keyboardKeys_t::KEY_SELECT;
		case VK_SNAPSHOT: return keyboardKeys_t::KEY_PRINT_SCREEN;
		case VK_INSERT: return keyboardKeys_t::KEY_INSERT;
		case VK_DELETE: return keyboardKeys_t::KEY_DELETE;
		case VK_HELP: return keyboardKeys_t::KEY_HELP;
		case '0': return keyboardKeys_t::KEY_N0;
		case '1': return keyboardKeys_t::KEY_N1;
		case '2': return keyboardKeys_t::KEY_N2;
		case '3': return keyboardKeys_t::KEY_N3;
		case '4': return keyboardKeys_t::KEY_N4;
		case '5': return keyboardKeys_t::KEY_N5;
		case '6': return keyboardKeys_t::KEY_N6;
		case '7': return keyboardKeys_t::KEY_N7;
		case '8': return keyboardKeys_t::KEY_N8;
		case '9': return keyboardKeys_t::KEY_N9;
		case 'A': return keyboardKeys_t::KEY_A;
		case 'B': return keyboardKeys_t::KEY_B;
		case 'C': return keyboardKeys_t::KEY_C;
		case 'D': return keyboardKeys_t::KEY_D;
		case 'E': return keyboardKeys_t::KEY_E;
		case 'F': return keyboardKeys_t::KEY_F;
		case 'G': return keyboardKeys_t::KEY_G;
		case 'H': return keyboardKeys_t::KEY_H;
		case 'I': return keyboardKeys_t::KEY_I;
		case 'J': return keyboardKeys_t::KEY_J;
		case 'K': return keyboardKeys_t::KEY_K;
		case 'L': return keyboardKeys_t::KEY_L;
		case 'M': return keyboardKeys_t::KEY_M;
		case 'N': return keyboardKeys_t::KEY_N;
		case 'O': return keyboardKeys_t::KEY_O;
		case 'P': return keyboardKeys_t::KEY_P;
		case 'Q': return keyboardKeys_t::KEY_Q;
		case 'R': return keyboardKeys_t::KEY_R;
		case 'S': return keyboardKeys_t::KEY_S;
		case 'T': return keyboardKeys_t::KEY_T;
		case 'U': return keyboardKeys_t::KEY_U;
		case 'V': return keyboardKeys_t::KEY_V;
		case 'W': return keyboardKeys_t::KEY_W;
		case 'X': return keyboardKeys_t::KEY_X;
		case 'Y': return keyboardKeys_t::KEY_Y;
		case 'Z': return keyboardKeys_t::KEY_Z;
		case VK_LWIN: return keyboardKeys_t::KEY_L_COMMAND;
		case VK_RWIN: return keyboardKeys_t::KEY_R_COMMAND;
		case VK_SLEEP: return keyboardKeys_t::KEY_SLEEP;
		case VK_NUMPAD0: return keyboardKeys_t::KEY_NUMPAD_0;
		case VK_NUMPAD1: return keyboardKeys_t::KEY_NUMPAD_1;
		case VK_NUMPAD2: return keyboardKeys_t::KEY_NUMPAD_2;
		case VK_NUMPAD3: return keyboardKeys_t::KEY_NUMPAD_3;
		case VK_NUMPAD4: return keyboardKeys_t::KEY_NUMPAD_4;
		case VK_NUMPAD5: return keyboardKeys_t::KEY_NUMPAD_5;
		case VK_NUMPAD6: return keyboardKeys_t::KEY_NUMPAD_6;
		case VK_NUMPAD7: return keyboardKeys_t::KEY_NUMPAD_7;
		case VK_NUMPAD8: return keyboardKeys_t::KEY_NUMPAD_8;
		case VK_NUMPAD9: return keyboardKeys_t::KEY_NUMPAD_9;
		case VK_MULTIPLY: return keyboardKeys_t::KEY_NUMPAD_MULTIPLY;
		case VK_ADD: return keyboardKeys_t::KEY_NUMPAD_ADD;
		case VK_SEPARATOR: return keyboardKeys_t::KEY_NUMPAD_SEPARATOR;
		case VK_SUBTRACT: return keyboardKeys_t::KEY_NUMPAD_SUBTRACT;
		case VK_DECIMAL: return keyboardKeys_t::KEY_NUMPAD_DECIMAL;
		case VK_DIVIDE: return keyboardKeys_t::KEY_NUMPAD_DIVIDE;
		case VK_F1: return keyboardKeys_t::KEY_F1;
		case VK_F2: return keyboardKeys_t::KEY_F2;
		case VK_F3: return keyboardKeys_t::KEY_F3;
		case VK_F4: return keyboardKeys_t::KEY_F4;
		case VK_F5: return keyboardKeys_t::KEY_F5;
		case VK_F6: return keyboardKeys_t::KEY_F6;
		case VK_F7: return keyboardKeys_t::KEY_F7;
		case VK_F8: return keyboardKeys_t::KEY_F8;
		case VK_F9: return keyboardKeys_t::KEY_F9;
		case VK_F10: return keyboardKeys_t::KEY_F10;
		case VK_F11: return keyboardKeys_t::KEY_F11;
		case VK_F12: return keyboardKeys_t::KEY_F12;
		case VK_F13: return keyboardKeys_t::KEY_F13;
		case VK_F14: return keyboardKeys_t::KEY_F14;
		case VK_F15: return keyboardKeys_t::KEY_F15;
		case VK_F16: return keyboardKeys_t::KEY_F16;
		case VK_F17: return keyboardKeys_t::KEY_F17;
		case VK_F18: return keyboardKeys_t::KEY_F18;
		case VK_F19: return keyboardKeys_t::KEY_F19;
		case VK_F20: return keyboardKeys_t::KEY_F20;
		case VK_F21: return keyboardKeys_t::KEY_F21;
		case VK_F22: return keyboardKeys_t::KEY_F22;
		case VK_F23: return keyboardKeys_t::KEY_F23;
		case VK_F24: return keyboardKeys_t::KEY_F24;
		case VK_NUMLOCK: return keyboardKeys_t::KEY_NUM_LOCK;
		case VK_SCROLL: return keyboardKeys_t::KEY_SCROLL_LOCK;
		case VK_LSHIFT: return keyboardKeys_t::KEY_L_SHIFT;
		case VK_RSHIFT: return keyboardKeys_t::KEY_R_SHIFT;
		case VK_LCONTROL: return keyboardKeys_t::KEY_L_CTRL;
		case VK_RCONTROL: return keyboardKeys_t::KEY_R_CTRL;
		case VK_LMENU: return keyboardKeys_t::KEY_L_ALT;
		case VK_RMENU: return keyboardKeys_t::KEY_R_ALT;
		default: return keyboardKeys_t::KEY_INVALID;
	}
}

bool qpWindowsKeyboard::ProcessWindowEvent( UINT msg, WPARAM wparam, LPARAM lparam ) {
	switch( msg ) {
		case WM_KEYUP: {
			keyboardKeys_t key = FromVirtualKey( wparam );
			if ( key != keyboardKeys_t::KEY_INVALID ) {
				m_workingState.ClearBit( key );
				return true;
			}
			return false;
		}
		case WM_KEYDOWN: {
			keyboardKeys_t key = FromVirtualKey( wparam );
			if ( key != keyboardKeys_t::KEY_INVALID ) {
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

#endif
