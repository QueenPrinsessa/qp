#pragma once

namespace qp {
	class CameraComponent {
	public:
		float m_hfovDeg = 90.0f;
		float m_nearPlane = 1.0f;
		float m_farPlane = 100000.0f;
	};
}