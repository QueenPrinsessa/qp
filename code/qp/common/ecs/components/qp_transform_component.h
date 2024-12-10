#pragma once
#include "common/math/qp_quat.h"

class qpTransformComponent {
public:
	qpVec3 m_translation	= g_vec3Zero;
	qpVec3 m_rotation		= g_vec3Zero;
	qpQuat m_orientation	= g_quatIdentity;
};
