#pragma once
#if defined( QP_PLATFORM_WINDOWS )

#include "qp/engine/window/qp_mouse.h"
#include "qp/common/platform/windows/qp_types_win32.h"

class qpMouse_Win32 : public qpMouse {
	friend class qpWindow_Win32;
public:
	qpMouse_Win32() = default;
	virtual ~qpMouse_Win32() override = default;

private:
	bool ProcessWindowEvent( UINT msg, WPARAM wparam, LPARAM lparam ); // returns true if handled.
};

#endif