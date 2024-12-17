#pragma once

#if defined( QP_PLATFORM_SDL )
#include "engine/input/qp_cursor.h"
#include "qp/engine/platform/sdl/qp_sdl.h"

namespace qp {
	class MouseCursor_SDL : public Cursor {
		friend class Mouse_SDL;
	public:
		virtual void Update() override;

	private:
		bool HandleSDLEvent( const SDL_Event & event ); // returns true if handled.
		
		Vec2 m_nextDelta { 0, 0 };
	};
}

#endif
