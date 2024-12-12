#pragma once

#if !defined( QP_HEADLESS )

#include "qp/common/string/qp_string.h"

namespace qp {
	enum class windowMode_t {
		WINDOWED,
		BORDERLESS,
		COUNT
	};

	struct windowProperties_t {
		String title = "qpApp";
		int width = 1280;
		int height = 720;
		windowMode_t mode = windowMode_t::WINDOWED;
		bool allowResize = true;
		void * platformData = NULL;
	};
}
#endif