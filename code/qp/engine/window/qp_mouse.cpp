#include "engine.pch.h"

#if !defined( QP_HEADLESS )

#include "qp_mouse.h"

namespace qp {
	void Mouse::Update() {
		m_lastState = m_state;
		m_state = m_workingState;

		m_scrollValue = m_workingScrollValue;
		m_workingScrollValue.Zero();

		GetMouseCursorNonConst().Update();
	}

	bool Mouse::HandleEvent( const mouseButton_t button, const bool down ) {
		if ( QP_VERIFY( button != mouseButton_t::INVALID ) ) {
			m_workingState.SetBit( button, down );
			return true;
		}
		return false;
	}

	bool Mouse::HandleScrollEvent( const float scrollX, const float scrollY ) {
		m_workingScrollValue.x += scrollX;
		m_workingScrollValue.y += scrollY;
		return true;
	}
}
#endif