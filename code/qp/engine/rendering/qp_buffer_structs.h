#pragma once

#if !defined( QP_HEADLESS )

#include "qp/common/math/qp_mat.h"

struct uniformBufferObject_t {
	qpMat4 model;
	qpMat4 view;
	qpMat4 projection;
};

#endif