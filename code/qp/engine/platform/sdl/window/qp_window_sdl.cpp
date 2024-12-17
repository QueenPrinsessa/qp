#include "engine.pch.h"
#if defined( QP_PLATFORM_SDL )
#include "qp_window_sdl.h"

namespace qp {
	Window_SDL::Window_SDL( const windowProperties_t & properties ) {
		m_window = SDL_CreateWindow( properties.title.c_str(), properties.width, properties.height, SDL_WINDOW_VULKAN );
		SDL_GetWindowSize( m_window, &m_width, &m_height );
		SDL_GetWindowSizeInPixels( m_window, &m_clientHeight, &m_clientWidth );

		m_windowID = SDL_GetWindowID( m_window );
	}

	Window_SDL::~Window_SDL () {
		SDL_DestroyWindow( m_window );
	}

	void Window_SDL::OnUpdate() {
		m_mouse.Update();
		m_keyboard.Update();
	}

	void Window_SDL::PollEvents() {
		QP_ASSERT_MAIN_THREAD();

		SDL_Event event;
		while ( SDL_PollEvent( &event ) ) {
			HandleSDLEvent( event );
		}
	}

	bool Window_SDL::HandleSDLEvent ( const SDL_Event & event ) {
		if ( m_mouse.HandleSDLEvent( event ) ) {
			return true;
		}

		if ( m_keyboard.HandleSDLEvent( event ) ) {
			return true;
		}

		switch ( event.type ) {
			case SDL_EVENT_WINDOW_RESIZED: {
				QP_ASSERT_RELEASE( IsThisWindow( event.window.windowID ) );
				m_width = event.window.data1;
				m_height = event.window.data2;
				break;
			}
			case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED: {
				QP_ASSERT_RELEASE( IsThisWindow( event.window.windowID ) );
				m_clientWidth = event.window.data1;
				m_clientHeight = event.window.data2;
				break;
			}
			case SDL_EVENT_QUIT: {
				if ( m_destroyCallback ) {
					m_destroyCallback();
				}
				break;
			}
		}

		return false;
	}
}
#endif