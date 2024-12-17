#include "engine.pch.h"

#if defined( QP_PLATFORM_SDL )
#include "qp_sdl.h"

namespace qp {
	bool InitializeSDL() {
		QP_ASSERT_MAIN_THREAD();

		if ( atexit( SDL_Quit ) != 0 ) {
			qp::debug::Error( "Failed to setup SDL_Quit" );
			return false;
		}

		if ( !SDL_Init( SDL_INIT_EVENTS ) ) {
			qp::debug::Error( "%s", SDL_GetError() );
			return false;
		}
		if ( !SDL_InitSubSystem( SDL_INIT_VIDEO ) ) {
			qp::debug::Error( "%s", SDL_GetError() );
			return false;
		}
		return true;
	}
	void ShutdownSDL() { 
		SDL_QuitSubSystem( SDL_INIT_VIDEO );
		SDL_QuitSubSystem( SDL_INIT_EVENTS );
	}
}
#endif