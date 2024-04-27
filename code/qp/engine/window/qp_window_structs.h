#include "qp/common/string/qp_string.h"

enum class windowMode_t {
	WINDOWED,
	BORDERLESS,
	COUNT
};

struct windowProperties_t {
	qpString title = "qpApp";
	int width = 1280;
	int height = 720;
	windowMode_t mode = windowMode_t::WINDOWED;
	bool allowResize = true;
	void * platformData = NULL;
};