#include "engine.pch.h"
#include "qp_keyboard.h"

namespace qp {
	void Keyboard::Update() {
		m_lastState = m_state;
		m_state = m_workingState;
	}
}