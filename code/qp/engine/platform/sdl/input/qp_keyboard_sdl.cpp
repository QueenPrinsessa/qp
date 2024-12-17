#include "engine.pch.h"

#if defined( QP_PLATFORM_SDL )
#include "qp_keyboard_sdl.h"
#include "qp/engine/platform/sdl/qp_sdl.h"

namespace qp {
	constexpr keyCode_t MapScancodeToKeyCode( const SDL_Scancode key ) {
		switch ( key ) {
			case SDL_SCANCODE_BACKSPACE: return keyCode_t::BACKSPACE;
			case SDL_SCANCODE_TAB: return keyCode_t::TAB;
			case SDL_SCANCODE_CLEAR: return keyCode_t::CLEAR;
			case SDL_SCANCODE_RETURN: return keyCode_t::ENTER;
			case SDL_SCANCODE_PAUSE: return keyCode_t::PAUSE;
			case SDL_SCANCODE_CAPSLOCK: return keyCode_t::CAPS_LOCK;
			case SDL_SCANCODE_ESCAPE: return keyCode_t::ESCAPE;
			case SDL_SCANCODE_SPACE: return keyCode_t::SPACE;
			case SDL_SCANCODE_PRIOR: return keyCode_t::PAGE_UP;
			case SDL_SCANCODE_PAGEDOWN: return keyCode_t::PAGE_DOWN;
			case SDL_SCANCODE_END: return keyCode_t::END;
			case SDL_SCANCODE_HOME: return keyCode_t::HOME;
			case SDL_SCANCODE_LEFT: return keyCode_t::LEFT;
			case SDL_SCANCODE_UP: return keyCode_t::UP;
			case SDL_SCANCODE_RIGHT: return keyCode_t::RIGHT;
			case SDL_SCANCODE_DOWN: return keyCode_t::DOWN;
			case SDL_SCANCODE_SELECT: return keyCode_t::SELECT;
			case SDL_SCANCODE_PRINTSCREEN: return keyCode_t::PRINT_SCREEN;
			case SDL_SCANCODE_INSERT: return keyCode_t::INSERT;
			case SDL_SCANCODE_DELETE: return keyCode_t::DELETE;
			case SDL_SCANCODE_HELP: return keyCode_t::HELP;
			case SDL_SCANCODE_0: return keyCode_t::N0;
			case SDL_SCANCODE_1: return keyCode_t::N1;
			case SDL_SCANCODE_2: return keyCode_t::N2;
			case SDL_SCANCODE_3: return keyCode_t::N3;
			case SDL_SCANCODE_4: return keyCode_t::N4;
			case SDL_SCANCODE_5: return keyCode_t::N5;
			case SDL_SCANCODE_6: return keyCode_t::N6;
			case SDL_SCANCODE_7: return keyCode_t::N7;
			case SDL_SCANCODE_8: return keyCode_t::N8;
			case SDL_SCANCODE_9: return keyCode_t::N9;
			case SDL_SCANCODE_A: return keyCode_t::A;
			case SDL_SCANCODE_B: return keyCode_t::B;
			case SDL_SCANCODE_C: return keyCode_t::C;
			case SDL_SCANCODE_D: return keyCode_t::D;
			case SDL_SCANCODE_E: return keyCode_t::E;
			case SDL_SCANCODE_F: return keyCode_t::F;
			case SDL_SCANCODE_G: return keyCode_t::G;
			case SDL_SCANCODE_H: return keyCode_t::H;
			case SDL_SCANCODE_I: return keyCode_t::I;
			case SDL_SCANCODE_J: return keyCode_t::J;
			case SDL_SCANCODE_K: return keyCode_t::K;
			case SDL_SCANCODE_L: return keyCode_t::L;
			case SDL_SCANCODE_M: return keyCode_t::M;
			case SDL_SCANCODE_N: return keyCode_t::N;
			case SDL_SCANCODE_O: return keyCode_t::O;
			case SDL_SCANCODE_P: return keyCode_t::P;
			case SDL_SCANCODE_Q: return keyCode_t::Q;
			case SDL_SCANCODE_R: return keyCode_t::R;
			case SDL_SCANCODE_S: return keyCode_t::S;
			case SDL_SCANCODE_T: return keyCode_t::T;
			case SDL_SCANCODE_U: return keyCode_t::U;
			case SDL_SCANCODE_V: return keyCode_t::V;
			case SDL_SCANCODE_W: return keyCode_t::W;
			case SDL_SCANCODE_X: return keyCode_t::X;
			case SDL_SCANCODE_Y: return keyCode_t::Y;
			case SDL_SCANCODE_Z: return keyCode_t::Z;
			case SDL_SCANCODE_LGUI: return keyCode_t::L_CMD;
			case SDL_SCANCODE_RGUI: return keyCode_t::R_CMD;
			case SDL_SCANCODE_SLEEP: return keyCode_t::SLEEP;
			case SDL_SCANCODE_KP_0: return keyCode_t::NUMPAD_0;
			case SDL_SCANCODE_KP_1: return keyCode_t::NUMPAD_1;
			case SDL_SCANCODE_KP_2: return keyCode_t::NUMPAD_2;
			case SDL_SCANCODE_KP_3: return keyCode_t::NUMPAD_3;
			case SDL_SCANCODE_KP_4: return keyCode_t::NUMPAD_4;
			case SDL_SCANCODE_KP_5: return keyCode_t::NUMPAD_5;
			case SDL_SCANCODE_KP_6: return keyCode_t::NUMPAD_6;
			case SDL_SCANCODE_KP_7: return keyCode_t::NUMPAD_7;
			case SDL_SCANCODE_KP_8: return keyCode_t::NUMPAD_8;
			case SDL_SCANCODE_KP_9: return keyCode_t::NUMPAD_9;
			case SDL_SCANCODE_KP_MULTIPLY: return keyCode_t::NUMPAD_MULTIPLY;
			case SDL_SCANCODE_KP_PLUS: return keyCode_t::NUMPAD_ADD;
			case SDL_SCANCODE_SEPARATOR: return keyCode_t::NUMPAD_SEPARATOR;
			case SDL_SCANCODE_KP_MINUS: return keyCode_t::NUMPAD_SUBTRACT;
			case SDL_SCANCODE_KP_PERIOD: return keyCode_t::NUMPAD_DECIMAL;
			case SDL_SCANCODE_KP_DIVIDE: return keyCode_t::NUMPAD_DIVIDE;
			case SDL_SCANCODE_F1: return keyCode_t::F1;
			case SDL_SCANCODE_F2: return keyCode_t::F2;
			case SDL_SCANCODE_F3: return keyCode_t::F3;
			case SDL_SCANCODE_F4: return keyCode_t::F4;
			case SDL_SCANCODE_F5: return keyCode_t::F5;
			case SDL_SCANCODE_F6: return keyCode_t::F6;
			case SDL_SCANCODE_F7: return keyCode_t::F7;
			case SDL_SCANCODE_F8: return keyCode_t::F8;
			case SDL_SCANCODE_F9: return keyCode_t::F9;
			case SDL_SCANCODE_F10: return keyCode_t::F10;
			case SDL_SCANCODE_F11: return keyCode_t::F11;
			case SDL_SCANCODE_F12: return keyCode_t::F12;
			case SDL_SCANCODE_F13: return keyCode_t::F13;
			case SDL_SCANCODE_F14: return keyCode_t::F14;
			case SDL_SCANCODE_F15: return keyCode_t::F15;
			case SDL_SCANCODE_F16: return keyCode_t::F16;
			case SDL_SCANCODE_F17: return keyCode_t::F17;
			case SDL_SCANCODE_F18: return keyCode_t::F18;
			case SDL_SCANCODE_F19: return keyCode_t::F19;
			case SDL_SCANCODE_F20: return keyCode_t::F20;
			case SDL_SCANCODE_F21: return keyCode_t::F21;
			case SDL_SCANCODE_F22: return keyCode_t::F22;
			case SDL_SCANCODE_F23: return keyCode_t::F23;
			case SDL_SCANCODE_F24: return keyCode_t::F24;
			case SDL_SCANCODE_NUMLOCKCLEAR: return keyCode_t::NUM_LOCK;
			case SDL_SCANCODE_SCROLLLOCK: return keyCode_t::SCROLL_LOCK;
			case SDL_SCANCODE_LSHIFT: return keyCode_t::L_SHIFT;
			case SDL_SCANCODE_RSHIFT: return keyCode_t::R_SHIFT;
			case SDL_SCANCODE_LCTRL: return keyCode_t::L_CTRL;
			case SDL_SCANCODE_RCTRL: return keyCode_t::R_CTRL;
			case SDL_SCANCODE_LALT: return keyCode_t::L_ALT;
			case SDL_SCANCODE_RALT: return keyCode_t::R_ALT;
			default: return keyCode_t::INVALID; // we could map these to a reserved range between our max value + 256
		}
	}


	bool Keyboard_SDL::HandleSDLEvent ( const SDL_Event & event ) {
		QP_ASSERT_MAIN_THREAD();
		switch ( event.type ) {
			case SDL_EVENT_KEY_UP: {
				keyCode_t key = MapScancodeToKeyCode( event.key.scancode );
				return HandleEvent( key, false );
			}
			case SDL_EVENT_KEY_DOWN: {
				keyCode_t key = MapScancodeToKeyCode( event.key.scancode );
				return HandleEvent( key, true );
			}
		}
		return false;
	}
}

#endif
