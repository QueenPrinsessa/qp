#pragma once
#include "common/math/qp_vec.h"

namespace qp {
	class Cursor {
	public:
		virtual ~Cursor() = default;
		virtual void Update() = 0;

		const Vec2 & GetPosition() const { return m_position; }
		const Vec2 & GetPositionDelta() const { return m_positionDelta; }

		bool Show() { return ++m_displayCursor; }
		bool Hide() { return --m_displayCursor; }
		bool ForceHide() { return m_displayCursor = -1; }
		bool ForceShow() { return m_displayCursor = 0; }

		bool IsVisible() const { return m_displayCursor >= 0; }
	protected:
		Vec2 m_position;
		Vec2 m_previousPosition;
		Vec2 m_nextPosition;
		Vec2 m_positionDelta;
		atomicInt32_t m_displayCursor;
	};
}