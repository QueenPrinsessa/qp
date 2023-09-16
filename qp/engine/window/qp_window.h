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
	uint32 width = 1280;
	uint32 height = 720;
	qpWindowMode_t mode = qpWindowMode_t::WINDOWED;
	bool allowResize = true;
	void * platformData = NULL;
};

class qpWindow {
public:
	virtual ~qpWindow() = default;

	virtual void OnUpdate() = 0;

	virtual uint32 GetWidth() = 0;
	virtual uint32 GetHeight() = 0;

	virtual qpWindowMode_t GetWindowMode() const = 0;

	virtual void * GetHandle() const = 0;

};
