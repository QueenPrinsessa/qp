#include "engine.pch.h"

#if defined( QP_PLATFORM_SDL )
#include "qp_mouse_sdl.h"
#include "qp/engine/platform/sdl/qp_sdl.h"

namespace qp {
	constexpr mouseButton_t MapButtonToMouseButton( uint8 button ) {
		switch( button ) {
			case SDL_BUTTON_LEFT: return mouseButton_t::LBUTTON;
			case SDL_BUTTON_MIDDLE: return mouseButton_t::MBUTTON;
			case SDL_BUTTON_RIGHT: return mouseButton_t::RBUTTON;
			case SDL_BUTTON_X1: return mouseButton_t::MOUSE4;
			case SDL_BUTTON_X2: return mouseButton_t::MOUSE5;
		}
		return mouseButton_t::INVALID;
	}

	bool Mouse_SDL::HandleSDLEvent ( const SDL_Event & event ) {
		QP_ASSERT_MAIN_THREAD();
		if ( m_mouseCursor.HandleSDLEvent( event ) ) {
			return true;
		}

		switch ( event.type ) {
			case SDL_EVENT_MOUSE_BUTTON_UP: {
				mouseButton_t button = MapButtonToMouseButton( event.button.button );
				return HandleEvent( button, false );
			}
			case SDL_EVENT_MOUSE_BUTTON_DOWN: {
				mouseButton_t button = MapButtonToMouseButton( event.button.button );
				return HandleEvent( button, true );
			}
			case SDL_EVENT_MOUSE_WHEEL: {
				int flipped = event.wheel.direction == SDL_MOUSEWHEEL_FLIPPED ? -1 : 1;
				return HandleScrollEvent( event.wheel.x * flipped, event.wheel.y * flipped );
			}
		}

		return false;
	}
}

#endif
