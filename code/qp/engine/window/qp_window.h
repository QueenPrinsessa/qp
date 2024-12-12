#pragma once

#if !defined( QP_HEADLESS )

#include "qp/common/core/qp_types.h"
#include "qp/common/utilities/qp_function.h"
#include "qp/common/string/qp_string.h"
#include "qp_window_structs.h"

namespace qp {
	class Keyboard;
	class Mouse;

	class Window {
		using DestroyCallback = Function< void() >;
		using ResizeCallback = Function< void( int width, int height ) >;
	public:
		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual int GetWidth() const = 0;
		virtual int GetHeight() const = 0;
		virtual int GetClientWidth() const = 0;
		virtual int GetClientHeight() const = 0;

		virtual windowMode_t GetWindowMode() const = 0;

		virtual void * GetHandle() const = 0;

		virtual const Mouse & GetMouse() const = 0;
		virtual const Keyboard & GetKeyboard() const = 0;

		void SetDestroyCallback( DestroyCallback && callback ) { m_destroyCallback = Move( callback ); }
		void SetResizeCallback( ResizeCallback && callback ) { m_resizeCallback = Move( callback ); }
	protected:
		DestroyCallback m_destroyCallback;
		ResizeCallback m_resizeCallback;
	};
}
#endif