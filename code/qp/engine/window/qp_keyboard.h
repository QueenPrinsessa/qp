#pragma once
#include "qp_input_codes.h"
#include "common/containers/qp_array.h"
#include "qp/common/containers/qp_bitset.h"

namespace qp {
	class Keyboard {
	public:
		enum { KEYBOARD_KEY_COUNT = static_cast< int >( keyCode_t::COUNT ) };
		virtual ~Keyboard() = default;

		bool IsKeyDown( const keyCode_t key ) const { return m_state.GetBit( key ); }
		bool IsKeyPressed( const keyCode_t key ) const { return m_state.GetBit( key ) && !m_lastState.GetBit( key ); }
		bool IsKeyUp( const keyCode_t key ) const { return !m_state.GetBit( key ); }
		bool WasKeyReleased( const keyCode_t key ) const { return !m_state.GetBit( key ) && m_lastState.GetBit( key ); }

		void Update();

		BitSet< KEYBOARD_KEY_COUNT > GetState() const { return m_state; }
		BitSet< KEYBOARD_KEY_COUNT > GetLastState() const { return m_lastState; }
	protected:
		bool HandleEvent( const keyCode_t key, const bool down );
		void RegisterKeyCodeMapping( const uint16 platformKeyCode, const keyCode_t keyCode );
		keyCode_t MapPlatformKeyCode( const uint16 platformKeyCode ) const { return m_platformToKeyCode[ platformKeyCode ]; }
		uint16 MapKeyCode( const keyCode_t keyCode ) const { return m_keyCodeToPlatform[ static_cast< uint64 >( keyCode ) ]; }

		BitSet< KEYBOARD_KEY_COUNT > m_state {};
		BitSet< KEYBOARD_KEY_COUNT > m_lastState {};
		BitSet< KEYBOARD_KEY_COUNT > m_workingState {};
		Array< keyCode_t, KEYBOARD_KEY_COUNT > m_platformToKeyCode;
		Array< uint16, KEYBOARD_KEY_COUNT > m_keyCodeToPlatform;
	};
}