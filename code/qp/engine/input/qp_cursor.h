#pragma once
#include "common/math/qp_vec.h"

class qpCursor {
public:
	virtual ~qpCursor() = default;
	virtual void Update() = 0;

	const qpVec2i & GetPosition() const { return m_position; }
	const qpVec2i & GetPositionDelta() const { return m_positionDelta; }

	bool Show() { return ++m_displayCursor; }
	bool Hide() { return --m_displayCursor; }
	bool ForceHide() { return m_displayCursor = -1; }
	bool ForceShow() { return m_displayCursor = 0; }

	bool IsVisible() const { return m_displayCursor >= 0; }
protected:
	qpVec2i m_position;
	qpVec2i m_previousPosition;
	qpVec2i m_positionDelta;
	atomicInt32_t m_displayCursor;
};