#pragma once
#include "qp/common/math/qp_mat.h"

struct uniformBufferObject_t {
	qpMat4 model;
	qpMat4 view;
	qpMat4 projection;
};
