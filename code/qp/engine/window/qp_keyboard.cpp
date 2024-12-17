#include "engine.pch.h"
#include "qp_keyboard.h"

namespace qp {
	void Keyboard::Update() {
		m_lastState = m_state;
		m_state = m_workingState;
	}

	bool Keyboard::HandleEvent( const keyCode_t key, const bool down ) {
		if ( QP_VERIFY( key != keyCode_t::INVALID ) ) {
			m_workingState.SetBit( key, down );
			return true;
		}
		return false;
	}

	void Keyboard::RegisterKeyCodeMapping( const uint16 platformKeyCode, const keyCode_t keyCode ) {
		if ( !QP_VERIFY_RELEASE( platformKeyCode < KEYBOARD_KEY_COUNT ) ) {
			return;
		}
		if ( !QP_VERIFY_RELEASE( keyCode != keyCode_t::INVALID ) ) {
			return;
		}
		m_keyCodeToPlatform[ static_cast< uint64 >( keyCode ) ] = platformKeyCode;
		m_platformToKeyCode[ platformKeyCode ] = keyCode;
	}
	
}
