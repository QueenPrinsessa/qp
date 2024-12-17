#pragma once

#if defined( QP_PLATFORM_SDL )
#include "engine/window/qp_keyboard.h"
#include "qp/engine/platform/sdl/qp_sdl.h"

namespace qp {
	class Keyboard_SDL : public Keyboard {
		friend class Window_SDL;
	public:
	private:
		bool HandleSDLEvent( const SDL_Event & event ); // returns true if handled.
	};
}

#endif
