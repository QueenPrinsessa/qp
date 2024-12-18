#include "engine.pch.h"

#if defined( QP_PLATFORM_SDL )
#include "qp_mouse_cursor_sdl.h"

namespace qp {
	void MouseCursor_SDL::Update() {

		const bool regainedFocus = m_focused && !m_lastFocused;
		const bool lostFocus = !m_focused && m_lastFocused;
		m_lastFocused = m_focused;

		if ( lostFocus || regainedFocus ) {
			m_nextDelta.Zero();
		}

		m_position = m_nextPosition;
		m_delta = m_nextDelta;
		
		m_nextPosition = m_position;
		m_nextDelta.Zero();

		if ( IsVisible() ) {
			SDL_ShowCursor();
		} else {
			SDL_HideCursor();
		}

	}

	bool MouseCursor_SDL::HandleSDLEvent ( const SDL_Event & event ) {
		QP_ASSERT_MAIN_THREAD();
		switch ( event.type ) {
			case SDL_EVENT_WINDOW_FOCUS_GAINED: {
				m_focused = true;
				return false; // pass along
			}
			case SDL_EVENT_WINDOW_FOCUS_LOST: {
				m_focused = false;
				return false; // pass along
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
