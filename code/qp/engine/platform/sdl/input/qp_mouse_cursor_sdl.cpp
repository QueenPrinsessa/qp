#include "engine.pch.h"

#if defined( QP_PLATFORM_SDL )
#include "qp_mouse_cursor_sdl.h"

namespace qp {
	void MouseCursor_SDL::Update() {
		m_position = m_nextPosition;
		m_positionDelta = m_nextDelta;
		
		m_nextPosition = m_position;
		m_nextDelta.Zero();
	}

	bool MouseCursor_SDL::HandleSDLEvent ( const SDL_Event & event ) {
		QP_ASSERT_MAIN_THREAD();
		switch ( event.type ) {
			case SDL_EVENT_WINDOW_MOUSE_ENTER: {
			
			}
			case SDL_EVENT_WINDOW_MOUSE_LEAVE: {
			}
			case SDL_EVENT_MOUSE_MOTION: {
				m_nextPosition.x = event.motion.x;
				m_nextPosition.y = event.motion.y;
				m_nextDelta.x += event.motion.xrel;
				m_nextDelta.y += event.motion.yrel;
				return true;
			}
		}
		return false;
	}
}

#endif
