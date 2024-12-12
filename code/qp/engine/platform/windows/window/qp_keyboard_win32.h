#pragma once

#if !defined( QP_HEADLESS )

#if defined( QP_PLATFORM_WINDOWS )

#include "qp/engine/window/qp_keyboard.h"
#include "qp/common/platform/windows/qp_types_win32.h"

namespace qp {
	class Keyboard_Win32 : public Keyboard {
		friend class Window_Win32;
	public:
		Keyboard_Win32() = default;
		virtual ~Keyboard_Win32() override = default;

	private:
		bool ProcessWindowEvent( UINT msg, WPARAM wparam, LPARAM lparam ); // returns true if handled.
	};
}
#endif

#endif