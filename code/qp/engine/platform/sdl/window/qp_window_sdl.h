#pragma once

#if defined( QP_PLATFORM_SDL )
#include "qp/engine/window/qp_window.h"
#include "qp/engine/platform/sdl/qp_sdl.h"
#include "qp/engine/platform/sdl/input/qp_keyboard_sdl.h"
#include "qp/engine/platform/sdl/input/qp_mouse_sdl.h"

namespace qp {
	class Window_SDL : public Window {
	public:
		Window_SDL( const windowProperties_t & properties );
		virtual ~Window_SDL() override;

		virtual void OnUpdate() override;
		virtual void PollEvents() override;

		virtual int GetWidth() const override { return m_width; }
		virtual int GetHeight() const override { return m_height; }
		virtual int GetClientWidth() const override { return m_clientWidth; }
		virtual int GetClientHeight() const override { return m_clientHeight; }
		virtual windowMode_t GetWindowMode() const override { return m_windowMode; }
		virtual void * GetHandle() const override { return m_window; }
		virtual const Mouse & GetMouse() const override { return m_mouse; }
		virtual const Keyboard & GetKeyboard() const override { return m_keyboard; }

	private:
		bool HandleSDLEvent( const SDL_Event & event ); // returns true if handled.
		bool IsThisWindow( const SDL_WindowID windowID ) const { return windowID == m_windowID; }

		Mouse_SDL m_mouse;
		Keyboard_SDL m_keyboard;
		SDL_Window * m_window = NULL;
		SDL_WindowID m_windowID = 0;
		int m_width = 1280;
		int m_height = 720;
		int m_clientWidth = 1280;
		int m_clientHeight = 720;
		windowMode_t m_windowMode = windowMode_t::WINDOWED;
	};
}
#endif
