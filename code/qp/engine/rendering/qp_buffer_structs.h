#pragma once

#if !defined( QP_HEADLESS )

#include "qp/common/math/qp_mat.h"

namespace qp {
	struct uniformBufferObject_t {
		Mat4 model;
		Mat4 view;
		Mat4 projection;
	};
}
#endif