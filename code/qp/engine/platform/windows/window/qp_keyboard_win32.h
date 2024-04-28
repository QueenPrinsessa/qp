#pragma once

#if !defined( QP_HEADLESS )

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/engine/window/qp_keyboard.h"
#include "qp/common/platform/windows/qp_types_win32.h"

class qpKeyboard_Win32 : public qpKeyboard {
	friend class qpWindow_Win32;
public:
	qpKeyboard_Win32() = default;
	virtual ~qpKeyboard_Win32() override = default;

private:
	bool ProcessWindowEvent( UINT msg, WPARAM wparam, LPARAM lparam ); // returns true if handled.
};

#endif

#endif