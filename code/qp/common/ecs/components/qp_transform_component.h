#pragma once
#include "common/math/qp_quat.h"

namespace qp {
	class TransformComponent {
	public:
		Vec3 m_translation = g_vec3Zero;
		Vec3 m_rotation = g_vec3Zero;
		Quat m_orientation = g_quatIdentity;
	};
}