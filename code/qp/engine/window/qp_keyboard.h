#pragma once
#include "qp/common/containers/qp_bitset.h"

class qpKeyboard {
public:
	enum keyboardKeys_t {
		KEY_BACKSPACE,
		KEY_TAB,
		KEY_CLEAR,
		KEY_ENTER,
		KEY_SHIFT,
		KEY_CTRL,
		KEY_ALT,
		KEY_PAUSE,
		KEY_CAPS_LOCK,
		KEY_ESCAPE,
		KEY_SPACE,
		KEY_PAGE_UP,
		KEY_PAGE_DOWN,
		KEY_END,
		KEY_HOME,
		KEY_LEFT,
		KEY_UP,
		KEY_RIGHT,
		KEY_DOWN,
		KEY_SELECT,
		KEY_PRINT_SCREEN,
		KEY_INSERT,
		KEY_DELETE,
		KEY_HELP,
		KEY_N0,
		KEY_N1,
		KEY_N2,
		KEY_N3,
		KEY_N4,
		KEY_N5,
		KEY_N6,
		KEY_N7,
		KEY_N8,
		KEY_N9,
		KEY_A,
		KEY_B,
		KEY_C,
		KEY_D,
		KEY_E,
		KEY_F,
		KEY_G,
		KEY_H,
		KEY_I,
		KEY_J,
		KEY_K,
		KEY_L,
		KEY_M,
		KEY_N,
		KEY_O,
		KEY_P,
		KEY_Q,
		KEY_R,
		KEY_S,
		KEY_T,
		KEY_U,
		KEY_V,
		KEY_W,
		KEY_X,
		KEY_Y,
		KEY_Z,
		KEY_L_COMMAND, //Windows key, super key, cmd key
		KEY_R_COMMAND, //Windows key, super key, cmd key
		KEY_SLEEP,
		KEY_NUMPAD_0,
		KEY_NUMPAD_1,
		KEY_NUMPAD_2,
		KEY_NUMPAD_3,
		KEY_NUMPAD_4,
		KEY_NUMPAD_5,
		KEY_NUMPAD_6,
		KEY_NUMPAD_7,
		KEY_NUMPAD_8,
		KEY_NUMPAD_9,
		KEY_NUMPAD_MULTIPLY,
		KEY_NUMPAD_ADD,
		KEY_NUMPAD_SEPARATOR,
		KEY_NUMPAD_SUBTRACT,
		KEY_NUMPAD_DECIMAL,
		KEY_NUMPAD_DIVIDE,
		KEY_F1,
		KEY_F2,
		KEY_F3,
		KEY_F4,
		KEY_F5,
		KEY_F6,
		KEY_F7,
		KEY_F8,
		KEY_F9,
		KEY_F10,
		KEY_F11,
		KEY_F12,
		KEY_F13,
		KEY_F14,
		KEY_F15,
		KEY_F16,
		KEY_F17,
		KEY_F18,
		KEY_F19,
		KEY_F20,
		KEY_F21,
		KEY_F22,
		KEY_F23,
		KEY_F24,
		KEY_NUM_LOCK,
		KEY_SCROLL_LOCK,
		KEY_L_SHIFT,
		KEY_R_SHIFT,
		KEY_L_CTRL,
		KEY_R_CTRL,
		KEY_L_ALT,
		KEY_R_ALT,
		COUNT,
		KEY_INVALID = COUNT
	};

	virtual ~qpKeyboard() = default;

	bool IsKeyDown( const keyboardKeys_t key ) const { return m_state.GetBit( key ); }
	bool IsKeyPressed( const keyboardKeys_t key ) const { return m_state.GetBit( key ) && !m_lastState.GetBit( key ); }
	bool IsKeyUp( const keyboardKeys_t key ) const { return !m_state.GetBit( key ); }
	bool WasKeyReleased( const keyboardKeys_t key ) const { return !m_state.GetBit( key ) && m_lastState.GetBit( key ); }

	void Update();

	qpBitSet< keyboardKeys_t::COUNT > GetState() const { return m_state; }
	qpBitSet< keyboardKeys_t::COUNT > GetLastState() const { return m_lastState; }
protected:
	qpBitSet< keyboardKeys_t::COUNT > m_state {};
	qpBitSet< keyboardKeys_t::COUNT > m_lastState {};
	qpBitSet< keyboardKeys_t::COUNT > m_workingState {};
};
using keyboardKeys_t = qpKeyboard::keyboardKeys_t;
