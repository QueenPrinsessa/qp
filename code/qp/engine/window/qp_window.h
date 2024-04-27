#pragma once
#include "qp/common/core/qp_types.h"
#include "qp/common/utilities/qp_function.h"
#include "qp/common/string/qp_string.h"
#include "qp_window_structs.h"

class qpKeyboard;
class qpMouse;

class qpWindow {
	using DestroyCallback = qpFunction< void() >;
	using ResizeCallback = qpFunction< void( int width, int height) >;
public:
	virtual ~qpWindow() = default;

	virtual void OnUpdate() = 0;

	virtual int GetWidth() = 0;
	virtual int GetHeight() = 0;

	virtual windowMode_t GetWindowMode() const = 0;

	virtual void * GetHandle() const = 0;

	virtual const qpMouse & GetMouse() const = 0;
	virtual const qpKeyboard & GetKeyboard() const = 0;

	void SetDestroyCallback( DestroyCallback && callback ) { m_destroyCallback = qpMove( callback ); }
	void SetResizeCallback( ResizeCallback && callback ) { m_resizeCallback = qpMove( callback ); }
protected:
	DestroyCallback m_destroyCallback;
	ResizeCallback m_resizeCallback;
};
