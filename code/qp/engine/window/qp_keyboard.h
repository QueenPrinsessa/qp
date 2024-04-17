#pragma once
#include "qp_input_codes.h"
#include "qp/common/containers/qp_bitset.h"

class qpKeyboard {
public:
	enum { KEYBOARD_KEY_COUNT = static_cast< int >( keyboardKeys_t::COUNT ) };
	virtual ~qpKeyboard() = default;

	bool IsKeyDown( const keyboardKeys_t key ) const { return m_state.GetBit( key ); }
	bool IsKeyPressed( const keyboardKeys_t key ) const { return m_state.GetBit( key ) && !m_lastState.GetBit( key ); }
	bool IsKeyUp( const keyboardKeys_t key ) const { return !m_state.GetBit( key ); }
	bool WasKeyReleased( const keyboardKeys_t key ) const { return !m_state.GetBit( key ) && m_lastState.GetBit( key ); }

	void Update();

	qpBitSet< KEYBOARD_KEY_COUNT > GetState() const { return m_state; }
	qpBitSet< KEYBOARD_KEY_COUNT > GetLastState() const { return m_lastState; }
protected:
	qpBitSet< KEYBOARD_KEY_COUNT > m_state {};
	qpBitSet< KEYBOARD_KEY_COUNT > m_lastState {};
	qpBitSet< KEYBOARD_KEY_COUNT > m_workingState {};
};
