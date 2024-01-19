#include "engine.pch.h"
#include "qp_keyboard.h"

void qpKeyboard::Update() {
	m_lastState = m_state;
	m_state = m_workingState;
}
