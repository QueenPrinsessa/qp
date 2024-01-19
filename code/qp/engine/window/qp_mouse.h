#pragma once
#include "qp/common/containers/qp_bitset.h"

class qpMouse {
public:
	enum mouseButton_t {
		LBUTTON,
		RBUTTON,
		MBUTTON,
		MOUSE4,
		MOUSE5,
		COUNT
	};

	virtual ~qpMouse() = default;

	bool IsButtonDown( const mouseButton_t button ) const { return m_state.GetBit( button ); }
	bool IsButtonPressed( const mouseButton_t button ) const { return m_state.GetBit( button ) && !m_lastState.GetBit( button ); }
	bool IsButtonUp( const mouseButton_t button ) const { return !m_state.GetBit( button ); }
	bool WasButtonReleased( const mouseButton_t button ) const { return !m_state.GetBit( button ) && m_lastState.GetBit( button ); }

	void Update();

	qpBitSet< mouseButton_t::COUNT > GetState() const { return m_state; }
	qpBitSet< mouseButton_t::COUNT > GetLastState() const { return m_lastState; }
protected:
	qpBitSet< mouseButton_t::COUNT > m_state;
	qpBitSet< mouseButton_t::COUNT > m_lastState;
	qpBitSet< mouseButton_t::COUNT > m_workingState;
};

using mouseButton_t = qpMouse::mouseButton_t;
