#pragma once
#include "common/math/qp_mat.h"

class qpGameUpdateOutputComponent {
public:
	qpMat4 m_viewProjection = g_mat4Identity;
	qpVec3 m_viewTranslation = g_vec3Zero;
	qpQuat m_viewOrientation = g_quatIdentity;
};
