#pragma once
#include "engine/input/qp_cursor.h"

#if !defined( QP_HEADLESS )

#include "qp_input_codes.h"
#include "qp/common/containers/qp_bitset.h"
#include "qp/common/math/qp_vec.h"

namespace qp {
	class Mouse {
		enum { MOUSE_BUTTON_COUNT = static_cast< int >( mouseButton_t::COUNT ) };
	public:
		virtual ~Mouse() = default;

		bool IsButtonDown( const mouseButton_t button ) const { return m_state.GetBit( button ); }
		bool IsButtonPressed( const mouseButton_t button ) const { return m_state.GetBit( button ) && !m_lastState.GetBit( button ); }
		bool IsButtonUp( const mouseButton_t button ) const { return !m_state.GetBit( button ); }
		bool WasButtonReleased( const mouseButton_t button ) const { return !m_state.GetBit( button ) && m_lastState.GetBit( button ); }

		float GetScrollValue() const { return m_scrollValue.y; }
		float GetHorizontalScrollValue() const { return m_scrollValue.x; }

		virtual const Cursor & GetMouseCursor() const = 0;

		void Update();

		const BitSet< MOUSE_BUTTON_COUNT > & GetState() const { return m_state; }
		const BitSet< MOUSE_BUTTON_COUNT > & GetLastState() const { return m_lastState; }
	protected:
		BitSet< MOUSE_BUTTON_COUNT >	m_state;
		BitSet< MOUSE_BUTTON_COUNT >	m_lastState;
		BitSet< MOUSE_BUTTON_COUNT >	m_workingState;
		Vec2							m_scrollValue;
		Vec2							m_workingScrollValue;
	};
}
#endif