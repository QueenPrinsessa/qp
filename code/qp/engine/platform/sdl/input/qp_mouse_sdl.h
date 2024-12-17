#pragma once

#if defined( QP_PLATFORM_SDL )
#include "engine/window/qp_mouse.h"
#include "qp_mouse_cursor_sdl.h"
#include "qp/engine/platform/sdl/qp_sdl.h"

namespace qp {
	class Mouse_SDL : public Mouse {
		friend class Window_SDL;
	public:
		virtual const Cursor & GetMouseCursor() const { return m_mouseCursor; }
	private:
		bool HandleSDLEvent( const SDL_Event & event ); // returns true if handled.

		MouseCursor_SDL m_mouseCursor;
	};
}

#endif
