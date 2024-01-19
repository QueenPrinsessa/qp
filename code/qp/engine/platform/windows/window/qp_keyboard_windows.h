#pragma once

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/common/platform/windows/qp_windows.h"
#include "qp/engine/window/qp_keyboard.h"


class qpWindowsKeyboard : public qpKeyboard {
	friend class qpWindowsWindow;
public:
	qpWindowsKeyboard() = default;
	virtual ~qpWindowsKeyboard() override = default;

private:
	bool ProcessWindowEvent( UINT msg, WPARAM wparam, LPARAM lparam ); // returns true if handled.
};

#endif