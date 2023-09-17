#pragma once
#include "qp/common/core/qp_types.h"
#include "qp/tools/string/qp_string.h"

enum class qpWindowMode_t {
	WINDOWED,
	BORDERLESS,
	COUNT
};

struct qpWindowProperties_t {
	qpString title = "qpApp";
	int width = 1280;
	int height = 720;
	qpWindowMode_t mode = qpWindowMode_t::WINDOWED;
	bool allowResize = true;
	void * platformData = NULL;
};

class qpWindow {
public:
	virtual ~qpWindow() = default;

	virtual void OnUpdate() = 0;

	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;

	virtual qpWindowMode_t GetWindowMode() const = 0;

	virtual void * GetHandle() const = 0;

};
