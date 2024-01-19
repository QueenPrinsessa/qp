#pragma once
#if defined( QP_PLATFORM_WINDOWS )

#include "qp/common/platform/windows/qp_windows.h"
#include "qp/engine/window/qp_mouse.h"

class qpWindowsMouse : public qpMouse {
	friend class qpWindowsWindow;
public:
	qpWindowsMouse() = default;
	virtual ~qpWindowsMouse() override = default;

private:
	bool ProcessWindowEvent( UINT msg, WPARAM wparam, LPARAM lparam ); // returns true if handled.
};

#endif