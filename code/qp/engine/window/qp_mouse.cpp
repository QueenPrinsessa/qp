#include "engine.pch.h"
#include "qp_mouse.h"

void qpMouse::Update() {
	m_lastState = m_state;
	m_state = m_workingState;
}
