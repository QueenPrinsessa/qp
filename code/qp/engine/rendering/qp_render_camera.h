#pragma once
#include "common/math/qp_mat.h"
#include "common/math/qp_quat.h"

struct renderCamera_t {
	qpMat4 view = g_mat4Identity;
	qpMat4 projection = g_mat4Identity;
};

static void qpSetupRenderCamera( renderCamera_t & outRenderCamera, const qpVec3 & translation, const qpQuat & orientation, const qpMat4 & projection ) {
	outRenderCamera.view = qpRotationAndTranslationInverse( orientation.ToMat4() * qpCreateTranslation( translation ) ).Transposed();
	outRenderCamera.projection = projection.Transposed();
}