#include "engine.pch.h"

#if !defined( QP_HEADLESS )

#include "qp_mouse.h"

namespace qp {
	void Mouse::Update() {
		m_lastState = m_state;
		m_state = m_workingState;

		m_scrollValue = m_workingScrollValue;
		m_workingScrollValue.Zero();
	}
}
#endif