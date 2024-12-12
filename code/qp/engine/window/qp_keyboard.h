#pragma once
#include "qp_input_codes.h"
#include "qp/common/containers/qp_bitset.h"

namespace qp {
	class Keyboard {
	public:
		enum { KEYBOARD_KEY_COUNT = static_cast< int >( keyboardKeys_t::COUNT ) };
		virtual ~Keyboard() = default;

		bool IsKeyDown( const keyboardKeys_t key ) const { return m_state.GetBit( key ); }
		bool IsKeyPressed( const keyboardKeys_t key ) const { return m_state.GetBit( key ) && !m_lastState.GetBit( key ); }
		bool IsKeyUp( const keyboardKeys_t key ) const { return !m_state.GetBit( key ); }
		bool WasKeyReleased( const keyboardKeys_t key ) const { return !m_state.GetBit( key ) && m_lastState.GetBit( key ); }

		void Update();

		BitSet< KEYBOARD_KEY_COUNT > GetState() const { return m_state; }
		BitSet< KEYBOARD_KEY_COUNT > GetLastState() const { return m_lastState; }
	protected:
		BitSet< KEYBOARD_KEY_COUNT > m_state {};
		BitSet< KEYBOARD_KEY_COUNT > m_lastState {};
		BitSet< KEYBOARD_KEY_COUNT > m_workingState {};
	};
}