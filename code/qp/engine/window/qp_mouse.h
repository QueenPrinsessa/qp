#pragma once
#include "engine/input/qp_cursor.h"

#if !defined( QP_HEADLESS )

#include "qp_input_codes.h"
#include "qp/common/containers/qp_bitset.h"
#include "qp/common/math/qp_vec.h"

class qpMouse {
	enum { MOUSE_BUTTON_COUNT = static_cast< int >( mouseButton_t::COUNT ) };
public:
	virtual ~qpMouse() = default;

	bool IsButtonDown( const mouseButton_t button ) const { return m_state.GetBit( button ); }
	bool IsButtonPressed( const mouseButton_t button ) const { return m_state.GetBit( button ) && !m_lastState.GetBit( button ); }
	bool IsButtonUp( const mouseButton_t button ) const { return !m_state.GetBit( button ); }
	bool WasButtonReleased( const mouseButton_t button ) const { return !m_state.GetBit( button ) && m_lastState.GetBit( button ); }

	float GetScrollValue() const { return m_scrollValue.y; }
	float GetHorizontalScrollValue() const { return m_scrollValue.x; }

	virtual const qpCursor & GetMouseCursor() const = 0;

	void Update();

	const qpBitSet< MOUSE_BUTTON_COUNT > & GetState() const { return m_state; }
	const qpBitSet< MOUSE_BUTTON_COUNT > & GetLastState() const { return m_lastState; }
protected:
	qpBitSet< MOUSE_BUTTON_COUNT >	m_state;
	qpBitSet< MOUSE_BUTTON_COUNT >	m_lastState;
	qpBitSet< MOUSE_BUTTON_COUNT >	m_workingState;
	qpVec2							m_scrollValue;
	qpVec2							m_workingScrollValue;
};

#endif