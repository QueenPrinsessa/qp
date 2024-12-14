#pragma once
#include "common/math/qp_mat.h"
#include "common/math/qp_quat.h"

namespace qp {
	struct renderCamera_t {
		Mat4 view = g_mat4Identity;
		Mat4 projection = g_mat4Identity;
	};

	static void SetupRenderCamera( renderCamera_t & outRenderCamera, const Vec3 & translation, const Quat & orientation, const Mat4 & projection ) {
		outRenderCamera.view = RotationAndTranslationInverse( orientation.ToMat4() * CreateTranslation( translation ) );
		outRenderCamera.projection = projection;
	}
}