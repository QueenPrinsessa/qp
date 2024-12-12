#pragma once
#include "common/math/qp_mat.h"

namespace qp {
	namespace game {
		class UpdateOutputComponent {
		public:
			Mat4 m_viewProjection = g_mat4Identity;
			Vec3 m_viewTranslation = g_vec3Zero;
			Quat m_viewOrientation = g_quatIdentity;
		};
	}
}
